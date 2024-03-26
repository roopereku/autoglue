#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/EntityContext.hh>

#include <autoglue/FunctionEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/EnumEntity.hh>
#include <autoglue/EnumEntryEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/PrimitiveEntity.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/EnumEntity.hh>

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/ArgumentsAdjusters.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>

static std::shared_ptr <ag::TypeEntity> getPrimitive(clang::QualType type)
{
	if(type->isVoidType())
	{
		return ag::PrimitiveEntity::getVoid();
	}

	else if(type->isBooleanType())
	{
		return ag::PrimitiveEntity::getBoolean();
	}

	else if(type->isAnyCharacterType())
	{
		if(type->isPointerType())
		{
			return ag::PrimitiveEntity::getString();
		}

		return ag::PrimitiveEntity::getCharacter();
	}

	else if(type->isIntegerType())
	{
		return ag::PrimitiveEntity::getInteger();
	}

	else if(type->isRealType())
	{
		return ag::PrimitiveEntity::getDouble();
	}

	else if(type->isFloatingType())
	{
		return ag::PrimitiveEntity::getFloat();
	}

	if(type->isObjectType())
	{
		return ag::PrimitiveEntity::getObjectHandle();
	}

	assert(false);
	return nullptr;
}

class NodeVisitor : public clang::RecursiveASTVisitor <NodeVisitor>
{
public:
	NodeVisitor(ag::clang::Backend& backend, clang::SourceManager& sourceManager)
		: backend(backend), sourceManager(sourceManager)
	{
	}

	// TODO: Somehow make RecordDecl work.
	bool TraverseCXXRecordDecl(clang::CXXRecordDecl* decl)
	{
		ensureClassExists(decl);
		return true;
	}

	bool TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* decl)
	{
		ensureClassExists(decl);
		return true;
	}

	bool TraverseTypeAliasDecl(clang::TypeAliasDecl* decl)
	{
		ensureTypeAliasExists(decl);
		return true;
	}

	bool TraverseEnumDecl(clang::EnumDecl* decl)
	{
		bool created;
		auto entity = ensureEntityExists(decl, created);
		assert(entity);

		// If the enum existed already, don't add the constants
		// as they should already exist.
		if(!created)
		{
			return true;
		}

		// Iterate the constants and add them as children.
		for(auto value : decl->enumerators())
		{
			// Save the enum constant value to a string.
			llvm::SmallVector<char, 20> valueStr;
			value->getInitVal().toString(valueStr);

			// Add an enum entry that has the integer format.
			entity->addChild(std::make_shared <ag::EnumEntryEntity> (
				value->getNameAsString(),
				std::string(valueStr.begin(), valueStr.end()),
				ag::EnumEntryEntity::Format::Integer)
			);
		}

		return true;
	}

	bool TraverseFunctionDecl(clang::FunctionDecl* decl)
	{
		ensureFunctionExists(decl, ag::FunctionEntity::Type::Function);
		return true;
	}

	bool TraverseCXXMethodDecl(clang::CXXMethodDecl* decl)
	{
		ensureFunctionExists(decl, ag::FunctionEntity::Type::MemberFunction);
		return true;
	}

	bool TraverseCXXConstructorDecl(clang::CXXConstructorDecl* decl)
	{
		ensureFunctionExists(decl, ag::FunctionEntity::Type::Constructor);
		return true;
	}

	bool TraverseCXXDestructorDecl(clang::CXXDestructorDecl* decl)
	{
		ensureFunctionExists(decl, ag::FunctionEntity::Type::Destructor);
		return true;
	}

private:
	std::shared_ptr <ag::TypeEntity> resolveType(clang::QualType type)
	{
		auto underlying = type.getNonReferenceType();

		// If the type class of the underlying type is "elaborated", there might be a type alias underneath.
		if(underlying->getTypeClass() == clang::Type::TypeClass::Elaborated)
		{
			// Get the named type that the elaboration is referring to.
			auto referred = clang::cast <clang::ElaboratedType> (underlying.getTypePtrOrNull())->getNamedType();

			// Check if the referred type is a type alias.
			if(auto* alias = referred->getAs <clang::TypedefType> ())
			{
				return ensureTypeAliasExists(alias->getDecl());
			}
		}

		// TODO: Handle template instantiation references. For example when
		// shared_ptr is used it resolves the template class instead of an instantation here.
		if(auto tagDecl = underlying->getAsTagDecl())
		{
			auto result = ensureEntityExists(tagDecl);
			if(!result)
			{
				return nullptr;
			}

			return std::static_pointer_cast <ag::TypeEntity> (result);
		}

		else if(underlying->isBuiltinType())
		{
			auto name = type.getAsString();
			return getPrimitive(underlying);
		}

		static auto invalid = std::make_shared <ag::ClassEntity> ("invalidType");
		return invalid;
	}

	std::shared_ptr <ag::Entity> ensureEntityExists(clang::DeclContext* decl, unsigned depth = 0)
	{
		bool created;
		return ensureEntityExists(decl, created, depth);
	}

	std::shared_ptr <ag::Entity> ensureEntityExists(clang::DeclContext* decl, bool& created, unsigned depth = 0)
	{
		// Translation unit is the global scope.
		if(clang::dyn_cast <clang::TranslationUnitDecl> (decl))
		{
			return backend.getRootPtr();
		}

		// Don't return a valid node if a function is being treated as a parent.
		if(depth > 0 && clang::dyn_cast <clang::FunctionDecl> (decl))
		{
			return nullptr;
		}

		auto parent = ensureEntityExists(decl->getParent(), depth + 1);
		if(!parent)
		{
			return nullptr;
		}

		if(auto* named = clang::dyn_cast <clang::NamedDecl> (decl))
		{
			std::string name = named->getNameAsString();

			// If the declaration is a template instantiation, append template arguments to the name.
			if(auto* instantiation = clang::dyn_cast <clang::ClassTemplateSpecializationDecl> (named))
			{
				name += "__args";

				auto& args = instantiation->getTemplateArgs();
				for(unsigned i = 0; i < args.size(); i++)
				{
					// TODO: Somehow exclude arguments with default values.

					if(args[i].getKind() == clang::TemplateArgument::Type)
					{
						auto argType = resolveType(args[i].getAsType());

						if(argType)
						{
							name += + "_" + argType->getName();
						}
					}

					// TODO: Handle literals as arguments.
				}
			}

			auto result = parent->resolve(name);

			if(!result)
			{
				// Handle structs and classes.
				if(clang::dyn_cast <clang::RecordDecl> (named))
				{
					// Handle template instantiations.
					if(auto* instantiation = clang::dyn_cast <clang::ClassTemplateSpecializationDecl> (named))
					{
						parent->addChild(std::make_shared <ag::ClassEntity> (name));
					}

					else
					{
						// TODO: Filter out unions?
						parent->addChild(std::make_shared <ag::ClassEntity> (name));
					}
				}

				// Handle namespaces.
				else if(clang::dyn_cast <clang::NamespaceDecl> (named))
				{
					parent->addChild(std::make_shared <ag::ScopeEntity> (name));
				}

				// Handle enums.
				else if(clang::dyn_cast <clang::EnumDecl> (named))
				{
					parent->addChild(std::make_shared <ag::EnumEntity> (name));
				}

				// Create a group for functions.
				else if(auto* function = clang::dyn_cast <clang::FunctionDecl> (named))
				{
					parent->addChild(std::make_shared <ag::FunctionGroupEntity> (name));
				}

				else
				{
					std::cout << "Unimplemented: " << named->getDeclKindName() << '\n';
					return nullptr;
				}

				result = parent->resolve(name);

				if(!result)
				{
					//std::cout << "BUG: Failed to add " << name << '\n';
					//assert(false);
				}

				created = true;
			}

			return result;
		}

		// For any non-named entity, return the parent which at latest is the global scope.
		return parent;
	}

	std::shared_ptr <ag::TypeAliasEntity> ensureTypeAliasExists(clang::TypedefNameDecl* decl)
	{
		// Since TypeAliasDecl isn't a DeclContext, ensureEntityExists cannot be called directly.
		auto parent = ensureEntityExists(decl->getDeclContext(), 1);
		if(!parent)
		{
			return nullptr;
		}

		auto result = parent->resolve(decl->getNameAsString());

		if(!result)
		{
			auto alias = std::make_shared <ag::TypeAliasEntity>
				(decl->getNameAsString(), resolveType(decl->getTypeSourceInfo()->getType()));

			// Get the include path for this type alias and associate it with the entity.
			alias->initializeContext(std::make_shared <ag::clang::EntityContext> (
				backend.getInclusion(sourceManager.getBufferName(
						sourceManager.getIncludeLoc(sourceManager.getFileID(decl->getLocation()))).str())));

			result = alias;
			parent->addChild(std::move(alias));
		}

		assert(result);
		return std::static_pointer_cast <ag::TypeAliasEntity> (result);
	}

	void ensureClassExists(clang::CXXRecordDecl* decl)
	{
		auto entity = ensureEntityExists(decl);

		// If no entity was returned or it already existed, do nothing.
		if(!entity)
		{
			return;
		}

		auto classEntity = std::static_pointer_cast <ag::ClassEntity> (entity);

		auto definition = decl->getDefinition();
		if(definition)
		{
			// Get the include path for this class definition and associate it with the entity.
			classEntity->initializeContext(std::make_shared <ag::clang::EntityContext> (
				backend.getInclusion(sourceManager.getBufferName(
						sourceManager.getIncludeLoc(sourceManager.getFileID(definition->getLocation()))).str())));

			for(auto base : definition->bases())
			{
				auto baseEntity = resolveType(base.getType());
				if(baseEntity)
				{
					classEntity->addBaseClass(std::static_pointer_cast <ag::ClassEntity> (baseEntity));
				}
			}
		}

		for(auto nestedDecl : decl->decls())
		{
			TraverseDecl(nestedDecl);
		}
	}

	void ensureFunctionExists(clang::FunctionDecl* decl, ag::FunctionEntity::Type type)
	{
		std::shared_ptr <ag::FunctionEntity> function;

		switch(type)
		{
			case ag::FunctionEntity::Type::Constructor:
			case ag::FunctionEntity::Type::Destructor:
			{
				// TODO: Allow protected.
				if(decl->getAccess() != clang::AccessSpecifier::AS_public)
				{
					return;
				}

				function = std::make_shared <ag::FunctionEntity> (decl->getNameAsString(), type);
				break;
			}

			case ag::FunctionEntity::Type::MemberFunction:
			{
				// TODO: Allow protected.
				if(decl->getAccess() != clang::AccessSpecifier::AS_public)
				{
					return;
				}
			}

			case ag::FunctionEntity::Type::Function:
			{
				auto returnType = std::make_shared <ag::TypeReferenceEntity>
					("", resolveType(decl->getReturnType()));

				function = std::make_shared <ag::FunctionEntity>
					(decl->getNameAsString(), type, std::move(returnType));

				break;
			}
		}

		auto groupEntity = ensureEntityExists(decl);
		if(!groupEntity)
		{
			return;
		}

		for(auto param : decl->parameters())
		{
			auto paramType = std::make_shared <ag::TypeReferenceEntity>
				(param->getNameAsString(), resolveType(param->getType()));

			function->addChild(std::move(paramType));
		}

		groupEntity->addChild(std::move(function));
	}

	clang::SourceManager& sourceManager;
	ag::clang::Backend& backend;
};

class HierarchyGenerator : public clang::ASTConsumer
{
public:
	HierarchyGenerator(clang::SourceManager& sourceManager, ag::clang::Backend& backend)
		: visitor(backend, sourceManager)
	{
	}

private:
	void HandleTranslationUnit(clang::ASTContext& context) override
	{
		visitor.TraverseDecl(context.getTranslationUnitDecl());
	}

	NodeVisitor visitor;
};

class HierarchyGeneratorAction : public clang::ASTFrontendAction
{
public:
	HierarchyGeneratorAction(ag::clang::Backend& backend)
		: backend(backend)
	{
	}

	std::unique_ptr <clang::ASTConsumer> CreateASTConsumer(
		clang::CompilerInstance& instance, clang::StringRef) final
	{
		return std::make_unique <HierarchyGenerator>
			(instance.getSourceManager(), backend);
	}

private:
	ag::clang::Backend& backend;
};

class HierarchyGeneratorFactory : public clang::tooling::FrontendActionFactory
{
public:
	HierarchyGeneratorFactory(ag::clang::Backend& backend)
		: backend(backend)
	{
	}

	std::unique_ptr <clang::FrontendAction> create() override
	{
		return std::make_unique <HierarchyGeneratorAction> (backend);
	}

private:
	ag::clang::Backend& backend;
};

namespace ag::clang
{

Backend::Backend(std::string_view compilationDatabasePath)
	: ag::Backend(std::make_shared <ScopeEntity> ())
{
	std::string err;
	database = ::clang::tooling::JSONCompilationDatabase::loadFromFile(
		compilationDatabasePath,
		err,
		::clang::tooling::JSONCommandLineSyntax::AutoDetect
	);

	if(!database)
	{
		std::cerr << err << "\n";
		return;
	}

	bool nextIsPath = false;
	for(auto& file : database->getAllCompileCommands())
	{
		for(auto& command : file.CommandLine)
		{
			bool prefixed = (command[0] == '-' && command[1] == 'I');

			if(nextIsPath || prefixed)
			{
				nextIsPath = false;

				// If there is a prefix, remove it.
				if(prefixed)
				{
					command = std::string(command.begin() + 2, command.end());
				}

				// Only save include paths that didn't exist already.
				auto it = std::find(includePaths.begin(), includePaths.end(), command);
				if(it == includePaths.end())
				{
					includePaths.emplace_back(std::move(command));
				}

				continue;
			}

			if(command == "-isystem")
			{
				nextIsPath = true;
			}
		}
	}

	// TODO: Do this only when explicitly specified by user.
	// This is done elsewhere by an argument adjuster which doesn't touch the database.
	includePaths.emplace_back("/lib/clang/16/include");

	// Sort the include paths by length. This is done to make sure that
	std::sort(
		includePaths.begin(), includePaths.end(),
		[](const std::string& s1, const std::string& s2)
		{
			return s2.size() < s1.size();
		}
	);

	//for(auto& path : includePaths)
	//{
	//	std::cout << "Path " << path << '\n';
	//}
}

std::string Backend::getInclusion(const std::string& path)
{
	for(auto& includePath : includePaths)
	{
		// Is the given include path present in the given path.
		if(path.find(includePath) != std::string::npos)
		{
			// Skip directory separators after the include path.
			// TODO: Support windows directory separators.
			size_t increment = 0;
			while(path[includePath.length() + increment] == '/')
			{
				increment++;
			}

			// If the include path is present, return the path without the prefix.
			return std::string(path.begin() + includePath.length() + increment, path.end());
		}
	}

	// Not a valid inclusion as it's not in a known include path.
	return std::string();
}

bool Backend::generateHierarchy()
{
	if(!database)
	{
		return false;
	}

	::clang::tooling::ClangTool tool(*database, database->getAllFiles());
	tool.setPrintErrorMessage(true);

	tool.appendArgumentsAdjuster(::clang::tooling::getClangStripOutputAdjuster());

	// TODO: Do this only when explicitly specified by user.
	tool.appendArgumentsAdjuster(::clang::tooling::getInsertArgumentAdjuster("-I/lib/clang/17/include/"));

	return tool.run(std::make_unique <HierarchyGeneratorFactory> (*this).get()) == 0;
}

}
