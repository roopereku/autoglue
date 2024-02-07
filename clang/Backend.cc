#include <gen/clang/Backend.hh>
#include <gen/clang/TypeContext.hh>

#include <gen/FunctionEntity.hh>
#include <gen/TypeReferenceEntity.hh>
#include <gen/EnumEntity.hh>
#include <gen/EnumEntryEntity.hh>
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
		ensureEntityExists(decl);

		for(auto nestedDecl : decl->decls())
		{
			TraverseDecl(nestedDecl);
		}

		return true;
	}

	bool TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* decl)
	{
		ensureEntityExists(decl);

		for(auto nestedDecl : decl->decls())
		{
			TraverseDecl(nestedDecl);
		}

		return true;

	}

	bool TraverseEnumDecl(clang::EnumDecl* decl)
	{
		ensureEntityExists(decl);
		return true;
	}

	bool TraverseCXXMethodDecl(clang::CXXMethodDecl* decl)
	{
		ensureEntityExists(decl);
		return true;
	}

	bool TraverseFunctionDecl(clang::FunctionDecl* decl)
	{
		ensureEntityExists(decl);
		return true;
	}

private:
	std::shared_ptr <gen::TypeEntity> resolveType(clang::QualType type)
	{
		// TODO: Handle template instantiation references. For example when
		// shared_ptr is used it resolves the template class instead of an instantation here.
		if(auto tagDecl = type->getAsTagDecl())
		{
			return std::static_pointer_cast <gen::TypeEntity> (ensureEntityExists(tagDecl));
		}

		else if(type->isBuiltinType())
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

	std::shared_ptr <gen::Entity> ensureEntityExists(clang::DeclContext* decl)
	{
		// Translation unit is the global scope.
		if(clang::dyn_cast <clang::TranslationUnitDecl> (decl))
		{
			return backend.getRootPtr();
		}

		auto parent = ensureEntityExists(decl->getParent());
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

						else
						{
							name += "_invalid";
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
						parent->addChild(std::make_shared <gen::ClassEntity> (named->getNameAsString()));
					}
				}

				// Handle namespaces.
				else if(clang::dyn_cast <clang::NamespaceDecl> (named))
				{
					parent->addChild(std::make_shared <gen::ScopeEntity> (named->getNameAsString()));
				}

				// Handle enums.
				else if(clang::dyn_cast <clang::EnumDecl> (named))
				{
					//std::cout << "Create enum " << named->getQualifiedNameAsString() << '\n';
					parent->addChild(std::make_shared <gen::EnumEntity> (named->getNameAsString()));
				}

				// Handle functions.
				else if(auto* function = clang::dyn_cast <clang::FunctionDecl> (named))
				{
					// Handle member functions.
					if(clang::dyn_cast <clang::CXXMethodDecl> (named))
					{
						// Handle constructors.
						if(clang::dyn_cast <clang::CXXConstructorDecl> (named))
						{
							parent->addChild(std::make_shared <gen::FunctionEntity>
								(named->getNameAsString(), gen::FunctionEntity::Type::Constructor));
						}

						// Handle destructors.
						else if(clang::dyn_cast <clang::CXXDestructorDecl> (named))
						{
							parent->addChild(std::make_shared <gen::FunctionEntity>
								(named->getNameAsString(), gen::FunctionEntity::Type::Destructor));
						}

						// Handle conversions.
						else if(clang::dyn_cast <clang::CXXConversionDecl> (named))
						{
							// TODO: Add conversion functions.
							return nullptr;
						}

						// Anything else should be a normal member function.
						else
						{
							auto returnType = std::make_shared <gen::TypeReferenceEntity>
								("", resolveType(function->getReturnType()));

							parent->addChild(std::make_shared <gen::FunctionEntity>
								(named->getNameAsString(), gen::FunctionEntity::Type::MemberFunction,
								 std::move(returnType)));
						}
					}

					// Anything else should be a non-member function.
					else
					{
						auto returnType = std::make_shared <gen::TypeReferenceEntity>
							("", resolveType(function->getReturnType()));

						parent->addChild(std::make_shared <gen::FunctionEntity>
							(named->getNameAsString(), gen::FunctionEntity::Type::Function,
							 std::move(returnType)));
					}
				}

				else
				{
					std::cout << "Unimplemented: " << named->getDeclKindName() << '\n';
					return nullptr;
				}

				result = parent->resolve(named->getNameAsString());
			}

			return result;
		}

		return nullptr;
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
