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

#include <filesystem>
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
		// TODO: This might not be enough when there are multiple pointers.
		// Make the type a non-pointer.
		if(type->isPointerType())
		{
			type = type->getPointeeType();

			// TODO: Maybe do this only for const types?
			// If the pointer type is a char pointer, it's a string.
			if(type->isCharType())
			{
				return ag::PrimitiveEntity::getString();
			}
		}

		type = type.getNonReferenceType();

		if(type->isBuiltinType())
		{
			return getPrimitive(type);
		}

		else if(type->isTypedefNameType())
		{
			auto* typedefNode = type->getAs <clang::TypedefType> ();

			if(typedefNode)
			{
				auto result = ensureEntityExists(typedefNode->getDecl());
				return result ? std::static_pointer_cast <ag::TypeEntity> (result) : nullptr;
			}

			// TODO: This should probably never happen.
			else
			{
				std::cerr << "Unable to get the decl for typedef " << type.getAsString() << '\n';
			}
		}

		else if(type->isStructureOrClassType() || type->isEnumeralType())
		{
			auto* tagNode = type->getAsTagDecl();
			auto result = ensureEntityExists(tagNode);

			return result ? std::static_pointer_cast <ag::TypeEntity> (result) : nullptr;
		}

		return nullptr;
	}

	std::shared_ptr <ag::Entity> ensureEntityExists(clang::Decl* decl, bool& created)
	{
		// If the declaration has no name, return the global scope entity.
		auto named = clang::dyn_cast <clang::NamedDecl> (decl);
		if(!named)
		{
			return backend.getRootPtr();
		}

		std::shared_ptr <ag::Entity> parentEntity = backend.getRootPtr();

		// If the parent node of this declaration is another declaration, make sure that it exists.
		auto parentDecl = clang::dyn_cast <clang::Decl> (named->getDeclContext());
		if(parentDecl)
		{
			parentEntity = ensureEntityExists(parentDecl);

			// If the parent doesn't exist at this point, add nothing.
			if(!parentEntity)
			{
				return nullptr;
			}
		}

		std::string name = named->getNameAsString();

		// If the node represents a template instantiation, it will have a different name.
		if(auto* templateDecl = clang::dyn_cast <clang::ClassTemplateSpecializationDecl> (named))
		{
			// Append all of the template arguments after the name.
			const auto& args = templateDecl->getTemplateArgs();
			for(size_t i = 0; i < args.size(); i++)
			{
				if(args[i].getKind() == clang::TemplateArgument::ArgKind::Type)
				{
					auto argType = resolveType(args[i].getAsType());
					if(argType)
					{
						name += '_' + argType->getName();
					}

					else
					{
						name += "_invalid";
					}
				}
			}
		}

		// If the parent entity doesn't contain an entity of the given name, try to add it.
		auto result = parentEntity->resolve(name);

		if(!result)
		{
			// Check if the declaration is a typedef.
			if(auto* typedefNode = clang::dyn_cast <clang::TypedefNameDecl> (named))
			{
				auto underlying = resolveType(typedefNode->getUnderlyingType());
				if(!underlying)
				{
					return nullptr;
				}

				parentEntity->addChild(std::make_shared <ag::TypeAliasEntity> (name, underlying));
			}

			// Check if the declaration is a class or a struct.
			else if(auto* recordNode = clang::dyn_cast <clang::RecordDecl> (named))
			{
				parentEntity->addChild(std::make_shared <ag::ClassEntity> (name));
			}

			// Check if the declaration is a namespace.
			else if(auto* namespaceNode = clang::dyn_cast <clang::NamespaceDecl> (named))
			{
				parentEntity->addChild(std::make_shared <ag::ScopeEntity> (name));
			}

			// Check if the declaration is a function.
			else if(auto* functionNode = clang::dyn_cast <clang::FunctionDecl> (named))
			{
				// When a function is handled by this function, a function group is added instead.
				// Separate overloads will be added by ensureFunctionExists.
				parentEntity->addChild(std::make_shared <ag::FunctionGroupEntity> (name));
			}

			// Check if the declaration is an enum.
			else if(auto* enumNode = clang::dyn_cast <clang::EnumDecl> (named))
			{
				auto enumEntity = std::make_shared <ag::EnumEntity> (name);

				for(auto value : enumNode->enumerators())
				{
					// Save the enum constant value to a string.
					llvm::SmallVector<char, 20> valueStr;
					value->getInitVal().toString(valueStr);

					enumEntity->addChild(std::make_shared <ag::EnumEntryEntity> (
						value->getNameAsString(),
						std::string(valueStr.begin(), valueStr.end()),
						ag::EnumEntryEntity::Format::Integer
					));
				}

				parentEntity->addChild(std::move(enumEntity));
			}

			else
			{
				std::cerr << "Unimplemented: " << named->getDeclKindName() << '\n';
				return nullptr;
			}

			// If the new entity was added succesfully, we should be able to resolve it.
			created = true;
			result = parentEntity->resolve(name);

			if(result && isIncluded(named))
			{
				result->initializeContext(std::make_shared <ag::clang::EntityContext> (getDeclInclusion(named)));
			}
		}

		return result;
	}

	std::shared_ptr <ag::Entity> ensureEntityExists(clang::Decl* decl)
	{
		bool created;
		return ensureEntityExists(decl, created);
	}

	void ensureFunctionExists(clang::FunctionDecl* decl, ag::FunctionEntity::Type type)
	{
		// Only export functions that are included. This is done to only expose functions
		// that users would gain access to through a header inclusion.
		if(!isIncluded(decl))
		{
			return;
		}

		// TODO: Implement template function instantiations.
		if(decl->getTemplatedKind() != clang::FunctionDecl::TemplatedKind::TK_NonTemplate)
		{
			return;
		}

		auto returnType = resolveType(decl->getReturnType());

		if(!returnType)
		{
			std::cerr << "Unable to add function " << decl->getQualifiedNameAsString() <<
						": Failed to resolve return type (" << decl->getReturnType().getAsString() << ")\n";
			return;
		}

		auto entity = std::make_shared <ag::FunctionEntity>
			(decl->getNameAsString(), type, std::make_shared <ag::TypeReferenceEntity> ("", returnType));

		for(auto param : decl->parameters())
		{
			auto paramType = resolveType(param->getType());

			if(!paramType)
			{
				std::cerr << "Unable to add function " << decl->getQualifiedNameAsString() <<
						": Failed to resolve type for parameter " << param->getNameAsString() <<
						" (" << param->getType().getAsString() << ")\n";
				return;
			}

			entity->addChild(std::make_shared <ag::TypeReferenceEntity> (param->getNameAsString(), paramType));
		}

		auto group = ensureEntityExists(decl);

		if(!group)
		{
			std::cerr << "Unable to add function " << decl->getQualifiedNameAsString() <<
						": Failed to ensure that the function group exists\n";
			return;
		}

		group->addChild(std::move(entity));
	}

	std::string getDeclInclusion(clang::Decl* decl)
	{
		// Get the name of the file that was included to receive the given declaration.
		// Before it is passed to Backend::getInclusion, the path is made canonical
		// so that an absolute path with no relativity such as dots is passed in.
		return backend.getInclusion(
			std::filesystem::canonical(
				sourceManager.getBufferName(sourceManager.getIncludeLoc(sourceManager.getFileID(decl->getLocation()))).str()
			)
		);
	}

	bool isIncluded(clang::Decl* decl)
	{
		return sourceManager.getIncludeLoc(sourceManager.getFileID(decl->getLocation())).isValid();
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
	includePaths.emplace_back("/lib/clang/17/include");

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
