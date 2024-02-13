#include <gen/clang/Backend.hh>

#include <gen/FunctionEntity.hh>
#include <gen/TypeReferenceEntity.hh>
#include <gen/TypeAliasEntity.hh>
#include <gen/EnumEntity.hh>
#include <gen/EnumEntryEntity.hh>
#include <gen/FunctionGroupEntity.hh>
#include <gen/PrimitiveEntity.hh>
#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>

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

class NodeVisitor : public clang::RecursiveASTVisitor <NodeVisitor>
{
public:
	NodeVisitor(gen::clang::Backend& backend) : backend(backend)
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
		ensureEntityExists(decl);
		return true;
	}

	bool TraverseFunctionDecl(clang::FunctionDecl* decl)
	{
		ensureFunctionExists(decl, gen::FunctionEntity::Type::Function);
		return true;
	}

	bool TraverseCXXMethodDecl(clang::CXXMethodDecl* decl)
	{
		ensureFunctionExists(decl, gen::FunctionEntity::Type::MemberFunction);
		return true;
	}

	bool TraverseCXXConstructorDecl(clang::CXXConstructorDecl* decl)
	{
		ensureFunctionExists(decl, gen::FunctionEntity::Type::Constructor);
		return true;
	}

	bool TraverseCXXDestructorDecl(clang::CXXDestructorDecl* decl)
	{
		ensureFunctionExists(decl, gen::FunctionEntity::Type::Destructor);
		return true;
	}

private:
	std::shared_ptr <gen::TypeEntity> resolveType(clang::QualType type)
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

			return std::static_pointer_cast <gen::TypeEntity> (result);
		}

		else if(underlying->isBuiltinType())
		{
			auto name = type.getAsString();
			std::replace(name.begin(), name.end(), ' ', '_');

			auto result = backend.getRoot().resolve(name);

			if(!result)
			{
				backend.getRoot().addChild(std::make_shared <gen::PrimitiveEntity> (name));
				result = backend.getRoot().resolve(name);
			}

			return std::static_pointer_cast <gen::TypeEntity> (result);
		}

		static auto invalid = std::make_shared <gen::ClassEntity> ("invalidType");
		return invalid;
	}

	std::shared_ptr <gen::Entity> ensureEntityExists(clang::DeclContext* decl, unsigned depth = 0)
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
						parent->addChild(std::make_shared <gen::ClassEntity> (name));
					}

					else
					{
						// TODO: Filter out unions?
						parent->addChild(std::make_shared <gen::ClassEntity> (name));
					}
				}

				// Handle namespaces.
				else if(clang::dyn_cast <clang::NamespaceDecl> (named))
				{
					parent->addChild(std::make_shared <gen::ScopeEntity> (name));
				}

				// Handle enums.
				else if(clang::dyn_cast <clang::EnumDecl> (named))
				{
					parent->addChild(std::make_shared <gen::EnumEntity> (name));
				}

				// Create a group for functions.
				else if(auto* function = clang::dyn_cast <clang::FunctionDecl> (named))
				{
					parent->addChild(std::make_shared <gen::FunctionGroupEntity> (name));
				}

				else
				{
					std::cout << "Unimplemented: " << named->getDeclKindName() << '\n';
					return nullptr;
				}

				result = parent->resolve(name);

				if(!result)
				{
					std::cout << "BUG: Failed to add " << name << '\n';
					//assert(false);
				}
			}

			return result;
		}

		// For any non-named entity, return the parent which at latest is the global scope.
		return parent;
	}

	std::shared_ptr <gen::TypeAliasEntity> ensureTypeAliasExists(clang::TypedefNameDecl* decl)
	{
		// Since TypeAliasDecl isn't a DeclContext, ensureEntityExists cannot be called directly.
		auto parent = ensureEntityExists(decl->getDeclContext(), 1);
		assert(parent);

		auto result = parent->resolve(decl->getNameAsString());

		if(!result)
		{
			parent->addChild(std::make_shared <gen::TypeAliasEntity>
				(decl->getNameAsString(), resolveType(decl->getTypeSourceInfo()->getType())));

			result = parent->resolve(decl->getNameAsString());
		}

		assert(result);
		return std::static_pointer_cast <gen::TypeAliasEntity> (result);
	}

	void ensureClassExists(clang::CXXRecordDecl* decl)
	{
		auto entity = ensureEntityExists(decl);
		if(!entity)
		{
			return;
		}

		auto classEntity = std::static_pointer_cast <gen::ClassEntity> (entity);

		auto definition = decl->getDefinition();
		if(definition)
		{
			for(auto base : definition->bases())
			{
				auto baseEntity = resolveType(base.getType());
				if(baseEntity)
				{
					classEntity->addBaseClass(std::static_pointer_cast <gen::ClassEntity> (baseEntity));
				}
			}
		}

		for(auto nestedDecl : decl->decls())
		{
			TraverseDecl(nestedDecl);
		}
	}

	void ensureFunctionExists(clang::FunctionDecl* decl, gen::FunctionEntity::Type type)
	{
		auto groupEntity = ensureEntityExists(decl);
		if(!groupEntity)
		{
			return;
		}

		std::shared_ptr <gen::FunctionEntity> function;

		switch(type)
		{
			case gen::FunctionEntity::Type::Constructor:
			case gen::FunctionEntity::Type::Destructor:
			{
				function = std::make_shared <gen::FunctionEntity> (decl->getNameAsString(), type);
				break;
			}

			case gen::FunctionEntity::Type::MemberFunction:
			case gen::FunctionEntity::Type::Function:
			{
				auto returnType = std::make_shared <gen::TypeReferenceEntity>
					("", resolveType(decl->getReturnType()));

				function = std::make_shared <gen::FunctionEntity>
					(decl->getNameAsString(), type, std::move(returnType));

				break;
			}
		}

		for(auto param : decl->parameters())
		{
			function->addChild(std::make_shared <gen::TypeReferenceEntity>
				(param->getNameAsString(), resolveType(param->getType())));
		}

		groupEntity->addChild(std::move(function));
	}

	gen::clang::Backend& backend;
};

class HierarchyGenerator : public clang::ASTConsumer
{
public:
	HierarchyGenerator(clang::SourceManager& manager, gen::clang::Backend& backend)
		: visitor(backend)
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
	HierarchyGeneratorAction(gen::clang::Backend& backend)
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
	gen::clang::Backend& backend;
};

class HierarchyGeneratorFactory : public clang::tooling::FrontendActionFactory
{
public:
	HierarchyGeneratorFactory(gen::clang::Backend& backend)
		: backend(backend)
	{
	}

	std::unique_ptr <clang::FrontendAction> create() override
	{
		return std::make_unique <HierarchyGeneratorAction> (backend);
	}

private:
	gen::clang::Backend& backend;
};

namespace gen::clang
{

Backend::Backend(FileList& headers)
	: gen::Backend(std::make_shared <ScopeEntity> ())
{
	std::ofstream inclusionFile("inclusions.cc");

	// Write the header inclusions.
	for(auto& path : headers)
	{
		inclusionFile << "#include " << path << '\n';
	}
}

bool Backend::loadCompilationDatabase(std::string_view path)
{
	std::string err;
	database = ::clang::tooling::JSONCompilationDatabase::loadFromFile(
		path,
		err,
		::clang::tooling::JSONCommandLineSyntax::AutoDetect
	);

	if(!database)
	{
		std::cerr << err << "\n";
		return false;
	}

	return true;
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
	tool.appendArgumentsAdjuster(::clang::tooling::getInsertArgumentAdjuster("-I/lib/clang/16/include/"));

	return tool.run(std::make_unique <HierarchyGeneratorFactory> (*this).get()) == 0;
}

}
