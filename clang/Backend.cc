#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/TypeContext.hh>
#include <autoglue/clang/TyperefContext.hh>
#include <autoglue/clang/FunctionContext.hh>
#include <autoglue/clang/OverloadContext.hh>
#include <autoglue/clang/GlueGenerator.hh>

#include <autoglue/FunctionEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/CallableTypeEntity.hh>
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
#include <cassert>
#include <set>

// Holds class entitites which have an untrivial new operator.
// At the end of the hierarchy generation, the constructors and destructors
// of these classes and classes deriving them will be disabled.
static std::set <std::shared_ptr <ag::ClassEntity>> untrivialNew;

std::string toString(const llvm::APSInt& value)
{
	llvm::SmallVector<char, 20> valueStr;
	value.toString(valueStr);

	return std::string(valueStr.begin(), valueStr.end());
}

std::string getFullTypename(const clang::TypeDecl* decl)
{
	assert(decl);

	// Make getAsString output "bool" instead of "_Bool" and ignore "class".
	::clang::PrintingPolicy pp(::clang::LangOptions{});
	pp.SuppressTagKeyword = 1;
	pp.Bool = 1;

	clang::QualType qualified(decl->getTypeForDecl(), 0);
	return qualified.getAsString(pp);
}

ag::FunctionEntity::Type getFunctionType(const clang::FunctionDecl* decl)
{
	switch(decl->getKind())
	{
		case clang::Decl::Kind::CXXConstructor: return ag::FunctionEntity::Type::Constructor;
		case clang::Decl::Kind::CXXDestructor: return ag::FunctionEntity::Type::Destructor;
		case clang::Decl::Kind::CXXMethod: return ag::FunctionEntity::Type::MemberFunction;

		default: return ag::FunctionEntity::Type::Function;
	}
}

ag::FunctionEntity::OverloadedOperator getOverloadedOperator(clang::OverloadedOperatorKind kind, bool& compound)
{
	switch(kind)
	{
		// Non-compound operators.
		case clang::OverloadedOperatorKind::OO_Plus: return ag::FunctionEntity::OverloadedOperator::Addition;
		case clang::OverloadedOperatorKind::OO_Minus: return ag::FunctionEntity::OverloadedOperator::Subtraction;
		case clang::OverloadedOperatorKind::OO_Star: return ag::FunctionEntity::OverloadedOperator::Multiplication;
		case clang::OverloadedOperatorKind::OO_Slash: return ag::FunctionEntity::OverloadedOperator::Addition;
		case clang::OverloadedOperatorKind::OO_Percent: return ag::FunctionEntity::OverloadedOperator::Modulus;

		case clang::OverloadedOperatorKind::OO_Less: return ag::FunctionEntity::OverloadedOperator::Less;
		case clang::OverloadedOperatorKind::OO_Greater: return ag::FunctionEntity::OverloadedOperator::Greater;
		case clang::OverloadedOperatorKind::OO_EqualEqual: return ag::FunctionEntity::OverloadedOperator::Equal;
		case clang::OverloadedOperatorKind::OO_ExclaimEqual: return ag::FunctionEntity::OverloadedOperator::NotEqual;

		case clang::OverloadedOperatorKind::OO_LessLess: return ag::FunctionEntity::OverloadedOperator::BitwiseShiftLeft;
		case clang::OverloadedOperatorKind::OO_GreaterGreater: return ag::FunctionEntity::OverloadedOperator::BitwiseShiftRight;
		case clang::OverloadedOperatorKind::OO_Caret: return ag::FunctionEntity::OverloadedOperator::BitwiseXOR;

		// Compound operators.
		case clang::OverloadedOperatorKind::OO_PlusEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Addition;
		case clang::OverloadedOperatorKind::OO_MinusEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Subtraction;
		case clang::OverloadedOperatorKind::OO_StarEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Multiplication;
		case clang::OverloadedOperatorKind::OO_SlashEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Addition;
		case clang::OverloadedOperatorKind::OO_PercentEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Modulus;

		case clang::OverloadedOperatorKind::OO_LessEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Less;
		case clang::OverloadedOperatorKind::OO_GreaterEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::Greater;

		case clang::OverloadedOperatorKind::OO_LessLessEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::BitwiseShiftLeft;
		case clang::OverloadedOperatorKind::OO_GreaterGreaterEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::BitwiseShiftRight;
		case clang::OverloadedOperatorKind::OO_CaretEqual: compound = true; return ag::FunctionEntity::OverloadedOperator::BitwiseXOR;

		default: {}
	}

	return ag::FunctionEntity::OverloadedOperator::None;
}

bool isReferenceType(clang::QualType type)
{
	// Reference and pointer types can be treated as references.
	if(type->isReferenceType() || type->isPointerType())
	{
		// Check if the type is a char pointer (i.e. is it a string).
		auto nonReference = type.getNonReferenceType();
		if(nonReference->isPointerType())
		{
			// If this type is a string, only treat is as a string when
			// it explicitly is marked as a reference.
			auto nonPointer = nonReference->getPointeeType();
			if(nonPointer->isAnyCharacterType())
			{
				return type->isReferenceType();
			}
		}

		return true;
	}

	return false;
}

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

	return nullptr;
}

class NodeVisitor : public clang::RecursiveASTVisitor <NodeVisitor>
{
public:
	NodeVisitor(ag::clang::Backend& backend, clang::SourceManager& sourceManager)
		: backend(backend), sourceManager(sourceManager)
	{
	}

	// TODO: Handle non-methods.
	//bool TraverseFunctionDecl(clang::FunctionDecl* decl)
	//{
	//	ensureFunctionExists(decl, ag::FunctionEntity::Type::Function);
	//	return true;
	//}

	bool TraverseCXXMethodDecl(clang::CXXMethodDecl* decl)
	{
		// Make sure that this function group and its containing entities exist.
		// Because this function is a class member function, ensureEntityExists
		// will add the member functions in the containing class, including this one.
		ensureEntityExists(decl);

		// Try to resolve the return type. If it's a template type instantiation,
		// ensureEntityExists will eventually store it and save its instantiated members.
		resolveType(decl->getReturnType());

		// Try to resolve the parameter types. If they are template type instantiations,
		// ensureEntityExists will eventually store them and save their instantiated members.
		for(auto param : decl->parameters())
		{
			resolveType(param->getType());
		}

		return true;
	}

private:
	void appendTemplateArgs(std::string& name, const clang::NamedDecl* named)
	{
		if(auto* templateDecl = clang::dyn_cast <clang::ClassTemplateSpecializationDecl> (named))
		{
			// Append all of the template arguments after the name.
			const auto& args = templateDecl->getTemplateArgs();
			for(size_t i = 0; i < args.size(); i++)
			{
				// Add a delimiter before any argument but the first.
				if(i > 0)
				{
					name += '_';
				}

				switch(args[i].getKind())
				{
					case clang::TemplateArgument::ArgKind::Type:
					{
						auto type = args[i].getAsType();

						// TODO: Save other qualifiers like references and pointers to the abstract template name.
						auto argType = resolveType(type);
						name += argType ? argType->getName() : "invalidType";
						break;
					}

					case clang::TemplateArgument::ArgKind::Integral:
					{
						auto value = toString(args[i].getAsIntegral());
						name += value;
						break;
					}

					case clang::TemplateArgument::ArgKind::TemplateExpansion:
					{
						name += "cxxexpansion";
						break;
					}

					case clang::TemplateArgument::ArgKind::Expression:
					{
						name += "cxxexpr";
						break;
					}

					// TODO: Implement the rest.
					default: {}
				}
			}
		}
	}

	std::string getEntityName(const clang::NamedDecl* named)
	{
		std::string name = named->getNameAsString();

		if(auto* functionNode = clang::dyn_cast <clang::FunctionDecl> (named))
		{
			if(clang::dyn_cast <clang::CXXDestructorDecl> (functionNode))
			{
				name = "Destructor";
			}

			else if(clang::dyn_cast <clang::CXXConversionDecl> (functionNode))
			{
				// TODO: Implement type conversions in the simplified hierarchy.
				return std::string();
			}

			else if(functionNode->isOverloadedOperator())
			{
				name = "OperatorOverload" + std::to_string(functionNode->getOverloadedOperator());
			}
		}

		appendTemplateArgs(name, named);
		return name;
	}

	std::shared_ptr <ag::TypeEntity> resolveFunctionType(clang::QualType type)
	{
		// If the given type is a function with a prototype, try to create an entity for it.
		auto* functionType = type->getAs <clang::FunctionProtoType> ();
		if(!functionType)
		{
			return nullptr;
		}

		auto ret = resolveType(functionType->getReturnType());
		if(!ret)
		{
			return nullptr;
		}

		auto entity = std::make_shared <ag::CallableTypeEntity> (std::make_shared <ag::TypeReferenceEntity> (
			"",
			ret,
			isReferenceType(functionType->getReturnType())
		));

		size_t paramIndex = 0;
		for(unsigned i = 0; i < functionType->getNumParams(); i++)
		{
			auto paramType = functionType->getParamType(i);
			auto paramTypeEntity = resolveType(paramType);

			if(!paramTypeEntity)
			{
				return nullptr;
			}

			entity->addParameter(std::make_shared <ag::TypeReferenceEntity> (
				"param" + std::to_string(paramIndex),
				paramTypeEntity,
				isReferenceType(paramType)
			));
		}

		auto result = backend.getRoot().resolve(entity->getName());

		if(!result)
		{
			auto name = entity->getName();

			backend.getRoot().addChild(std::move(entity));
			result = backend.getRoot().resolve(name);

			assert(result);
		}

		assert(result && result->getType() == ag::Entity::Type::Type);
		return std::static_pointer_cast <ag::TypeEntity> (result);
	}

	std::shared_ptr <ag::TypeEntity> resolveType(clang::QualType type)
	{
		type = type.getNonReferenceType();
		type = type.getUnqualifiedType();

		if(type->isFunctionType())
		{
			return resolveFunctionType(type);
		}

		// TODO: Ignore void pointers until there's a nice way to
		// present them in the abstraction.
		if(type->isVoidPointerType())
		{
			return nullptr;
		}

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

				if(result)
				{
					return std::static_pointer_cast <ag::TypeEntity> (result);
				}
			}

			// TODO: This should probably never happen.
			else
			{
				//std::cerr << "Unable to get the decl for typedef " << type.getAsString() << '\n';
			}
		}

		else if(type->isStructureOrClassType() || type->isEnumeralType())
		{
			auto* tagNode = type->getAsTagDecl();
			auto result = ensureEntityExists(tagNode);

			if(result)
			{
				return std::static_pointer_cast <ag::TypeEntity> (result);
			}
		}

		return nullptr;
	}

	std::shared_ptr <ag::Entity> ensureEntityExists(const clang::Decl* decl, bool& created)
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

		// If the given declaration doesn't come from an inclusion, return the parent
		// entity instead which at latest should be the global scope.
		//
		// NOTE: The reason that this isn't done before resolving the parent is that
		// with glibcxx std::string seems to fail when the containing namespace __cx11
		// cannot be ensured due to it not being defined in a header? As a side effect
		// this completely ignores the __cxx11 namespace which wouldn't make sense for
		// MSVC anyways.
		if(!isIncluded(named))
		{
			return parentEntity;
		}

		std::string name = getEntityName(named);

		// Anonymous declarations might make sense in C and C++, but they might not in foreign languages.
		// Until there is a nice way to contain them in the simplified hierarchy, ignore them.
		if(name.empty())
		{
			return nullptr;
		}

		// If the parent entity doesn't contain an entity of the given name, try to add it.
		auto result = parentEntity->resolve(name);

		if(!result)
		{
			bool shouldGetTypeInfo = true;

			// Check if the declaration is a typedef.
			if(auto* typedefNode = clang::dyn_cast <clang::TypedefNameDecl> (named))
			{
				auto underlyingType = typedefNode->getUnderlyingType();
				auto underlyingEntity = resolveType(underlyingType);

				if(!underlyingEntity)
				{
					return nullptr;
				}

				parentEntity->addChild(std::make_shared <ag::TypeAliasEntity> (name, underlyingEntity));
			}

			// Check if the declaration is a class or a struct.
			else if(auto* recordNode = clang::dyn_cast <clang::RecordDecl> (named))
			{
				// If the RecordDecl describes a template and is not a template instantiation,
				// don't add it as foreign languages can't do anything with it.
				if(recordNode->getDescribedTemplate())
				{
					return nullptr;
				}

				parentEntity->addChild(std::make_shared <ag::ClassEntity> (name));
				shouldGetTypeInfo = false;
			}

			// Check if the declaration is a namespace.
			else if(clang::dyn_cast <clang::NamespaceDecl> (named))
			{
				parentEntity->addChild(std::make_shared <ag::ScopeEntity> (name));
				shouldGetTypeInfo = false;
			}

			// Check if the declaration is a function.
			else if(auto* functionNode = clang::dyn_cast <clang::FunctionDecl> (named))
			{
				auto group = std::make_shared <ag::FunctionGroupEntity> (name, getFunctionType(functionNode));
				group->initializeContext(std::make_shared <ag::clang::FunctionContext> (functionNode));

				// When a function is handled by this function, a function group is added instead.
				// Separate overloads will be added by ensureFunctionExists.
				parentEntity->addChild(group);
				shouldGetTypeInfo = false;
			}

			// Check if the declaration is an enum.
			else if(auto* enumNode = clang::dyn_cast <clang::EnumDecl> (named))
			{
				// TODO: Somehow expose the actual underlying enum type?
				auto enumEntity = std::make_shared <ag::EnumEntity> (name, ag::EnumEntity::Format::Integer);

				for(auto value : enumNode->enumerators())
				{
					enumEntity->addEntry(std::make_shared <ag::EnumEntryEntity> (
						value->getNameAsString(),
						toString(value->getInitVal())
					));
				}

				parentEntity->addChild(std::move(enumEntity));
			}

			else
			{
				//std::cerr << "Unimplemented: " << named->getDeclKindName() << '\n';
				return nullptr;
			}

			// If the new entity was added succesfully, we should be able to resolve it.
			created = true;
			result = parentEntity->resolve(name);

			if(shouldGetTypeInfo && result && isIncluded(named))
			{
				result->initializeContext(std::make_shared <ag::clang::TypeContext> (
					getDeclInclusion(named), getFullTypename(clang::dyn_cast <clang::TypeDecl> (named))
				));
			}
		}

		// Since Clang will only tell us the true location of any given class when a node
		// pointing to it has a definition, it has to be checked here.
		if(result && !result->getContext())
		{
			if(auto* recordNode = clang::dyn_cast <clang::RecordDecl> (named))
			{
				auto* def = recordNode->getDefinition();

				if(def)
				{
					result->initializeContext(std::make_shared <ag::clang::TypeContext> (
						getDeclInclusion(def),
						getFullTypename(def)
					));

					if(auto* cxxDef = clang::dyn_cast <clang::CXXRecordDecl> (def))
					{
						auto classEntity = std::static_pointer_cast <ag::ClassEntity> (result);

						// Since the class definition is available here, collect its base classes.
						for(auto base : cxxDef->bases())
						{
							auto baseEntity = resolveType(base.getType());
							if(!baseEntity)
							{
								//std::cerr << "Unable to add base type for " << def->getQualifiedNameAsString() <<
								//			": Failed to resolve type " << base.getType().getAsString() << '\n';
								continue;
							}

							classEntity->addBaseType(baseEntity);
						}
					}

					// Because Clang only seems to provide definitions for instantiated template functions
					// when accessed through a record definition, go through every declaration found
					// within this class and try to add them to the hierarchy.
					for(auto subDecl : def->decls())
					{
						// For whatever reason the first decl seems to be pointing to the class itself
						// be it a template or not. If this was not skipped, every class would contain
						// itself as a nested class.
						if(subDecl == *def->decls_begin())
						{
							continue;
						}

						if(auto* method = clang::dyn_cast <clang::FunctionDecl> (subDecl))
						{
							ensureFunctionExists(method);
						}

						else
						{
							ensureEntityExists(subDecl);
						}
					}
				}
			}
		}

		return result;
	}

	std::shared_ptr <ag::Entity> ensureEntityExists(const clang::Decl* decl)
	{
		bool created;
		return ensureEntityExists(decl, created);
	}

	void ensureFunctionExists(clang::FunctionDecl* decl)
	{
		// If this function is an overload of the "new" operator which has more than 1 argument,
		// the bridge functions will be unable to instantiate an object with new.
		if(decl->getOverloadedOperator() == clang::OverloadedOperatorKind::OO_New &&
			decl->getNumParams() > 1)
		{
			auto group = ensureEntityExists(decl);

			if(group)
			{
				// TODO: If an overload of operator "new" with a single argument is found, it's probably
				// fine to remove the containing class from untrivialNew because that class and its
				// derivatives would become trivially instantiable with "new".

				// Only if the overload happens inside a class, remember the class for later.
				if(group->getParent().getType() == ag::Entity::Type::Type &&
					static_cast <ag::TypeEntity&> (group->getParent()).getType() == ag::TypeEntity::Type::Class)
				{
					untrivialNew.emplace(static_cast <ag::ClassEntity&> (group->getParent()).shared_from_this());
				}

			}

			// Operator "new" shouldn't be exported anyways.
			return;
		}

		// Only export functions that are included. This is done to only expose functions
		// that users would gain access to through a header inclusion.
		if(!isIncluded(decl))
		{
			return;
		}

		bool isProtected = false;
		bool isOverride = false;
		std::weak_ptr <ag::FunctionGroupEntity> privateOverrides;

		// Only export public and protected member functions.
		if(auto* cxxDecl = clang::dyn_cast <clang::CXXMethodDecl> (decl))
		{
			if(decl->getAccess() != clang::AccessSpecifier::AS_public &&
				decl->getAccess() != clang::AccessSpecifier::AS_protected)
			{
				// If this function overrides another, it should be saved.
				for(auto overridden : cxxDecl->overridden_methods())
				{
					// If the overridden method is protected, treat this private override as protected.
					isProtected = overridden->getAccess() == clang::AccessSpecifier::AS_protected;
					isOverride = true;

					// If a function group can be created for the overriden method, save the
					// group entity for OverloadContext to store later.
					auto group = ensureEntityExists(overridden);
					if(group && group->getType() == ag::Entity::Type::FunctionGroup)
					{
						privateOverrides = std::static_pointer_cast <ag::FunctionGroupEntity> (group);
					}

					break;
				}

				if(!isOverride)
				{
					return;
				}
			}
		}

		// Don't export deleted functions as they cannot be called.
		if(decl->isDeleted())
		{
			return;
		}

		// TODO: Implement type conversion once they have an abstraction.
		if(decl->getKind() == clang::Decl::Kind::CXXConversion)
		{
			return;
		}

		// Ignore copy constructors.
		// TODO: Think of a way to nicely expose these to foreign languages.
		if(auto* constructorNode = clang::dyn_cast <clang::CXXConstructorDecl> (decl))
		{
			if(constructorNode->isCopyConstructor())
			{
				return;
			}
		}

		auto returnTypeEntity = resolveType(decl->getReturnType());

		if(!returnTypeEntity)
		{
			//std::cerr << "Unable to add function " << decl->getQualifiedNameAsString() <<
			//			": Failed to resolve return type (" << decl->getReturnType().getAsString() << ")\n";
			return;
		}

		auto returnEntity = std::make_shared <ag::TypeReferenceEntity> (
			"",
			returnTypeEntity,
			isReferenceType(decl->getReturnType())
		);

		returnEntity->initializeContext(std::make_shared <ag::clang::TyperefContext> (
			decl->getReturnType(), decl->getASTContext()
		));

		if(auto* cxxDecl = clang::dyn_cast <clang::CXXMethodDecl> (decl))
		{
			isOverride = cxxDecl->size_overridden_methods() > 0;
			isProtected = isProtected || cxxDecl->getAccess() == clang::AccessSpecifier::AS_protected;
		}

		auto entity = std::make_shared <ag::FunctionEntity> (
			std::move(returnEntity), decl->isVirtualAsWritten(), isOverride,
			decl->isPureVirtual(), decl->isStatic()
		);

		entity->initializeContext(std::make_shared <ag::clang::OverloadContext> (decl, privateOverrides));

		if(isProtected)
		{
			entity->setProtected();
		}

		// If the function is an operator overload, check which one it is.
		if(decl->isOverloadedOperator())
		{
			bool compound = false;
			auto overloadedOperator = getOverloadedOperator(decl->getOverloadedOperator(), compound);

			// If there is no abstraction for the given operator overload, don't add this function.
			if(overloadedOperator == ag::FunctionEntity::OverloadedOperator::None)
			{
				return;
			}

			entity->setOverloadedOperator(overloadedOperator, compound);
		}

		size_t paramIndex = 1;
		for(auto param : decl->parameters())
		{
			auto paramTypeEntity = resolveType(param->getType());

			if(!paramTypeEntity)
			{
				//std::cerr << "Unable to add function " << decl->getQualifiedNameAsString() <<
				//		": Failed to resolve type for parameter " << param->getNameAsString() <<
				//		" (" << param->getType().getAsString() << ")\n";
				return;
			}

			auto name = param->getNameAsString();

			auto paramEntity = std::make_shared <ag::TypeReferenceEntity> (
				name.empty() ? "param" + std::to_string(paramIndex) : name,
				paramTypeEntity,
				isReferenceType(param->getType())
			);

			paramEntity->initializeContext(std::make_shared <ag::clang::TyperefContext> (
				param->getType(), decl->getASTContext()
			));

			entity->addParameter(std::move(paramEntity));
			paramIndex++;
		}

		auto group = ensureEntityExists(decl);

		if(!group)
		{
			//std::cerr << "Unable to add function " << decl->getQualifiedNameAsString() <<
			//			": Failed to ensure that the function group exists\n";
			return;
		}

		std::static_pointer_cast <ag::FunctionGroupEntity> (group)->addOverload(std::move(entity));
	}

	std::string getDeclInclusion(const clang::NamedDecl* decl)
	{
		auto loc = decl->getLocation();

		if(loc.isInvalid())
		{
			return std::string();
		}

		auto filename = sourceManager.getFilename(loc);
		if(filename.empty())
		{
			return std::string();
		}

		// TODO: /bits/ isn't a thing in MSVC.
		// Some C++ standard library includes are presented as the one where
		// the implementation resides. In such a case getIncludeLoc can be
		// used to get the inclusion that was actually used in the source code.
		// E.g. <bits/fs_path.h> -> <filesystem>
		if(filename.contains("/bits/"))
		{
			loc = sourceManager.getIncludeLoc(sourceManager.getFileID(loc));
			filename = sourceManager.getBufferName(loc);

			// If the source location still points to a file in /bits/, return nothing at all.
			if(filename.contains("/bits/"))
			{
				return std::string();
			}
		}

		// Convert the path of the included file to its canonical format and check if it
		// has a matching prefix to some include directory specified in the compilation database.
		return backend.getInclusion(std::filesystem::canonical(sourceManager.getBufferName(loc).str()));
	}

	bool isIncluded(const clang::Decl* decl)
	{
		return sourceManager.getIncludeLoc(sourceManager.getFileID(decl->getLocation())).isValid();
	}

	ag::clang::Backend& backend;
	clang::SourceManager& sourceManager;
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
	includePaths.emplace_back("/lib/clang/18/include");

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
	tool.appendArgumentsAdjuster(::clang::tooling::getInsertArgumentAdjuster("-I/lib/clang/18/include/"));

	bool result = tool.run(std::make_unique <HierarchyGeneratorFactory> (*this).get()) == 0;

	if(result)
	{
		for(auto entity : untrivialNew)
		{
			disableUntrivialNew(*entity);
		}
	}

	untrivialNew.clear();
	return result;
}

void Backend::generateGlue()
{
	GlueGenerator glueGen(*this);
	glueGen.generateBindings(false);
}

void Backend::disableUntrivialNew(ClassEntity& entity)
{
	// Disable constructors for classes that can't be trivially constrcucted with new
	auto constructors = entity.resolve("Constructor");
	if(constructors)
	{
		constructors->disableNewUsages();
	}

	// Since constructors are disabled, disable constructors when there is nothing deallocate.
	auto destructors = entity.resolve("Destructor");
	if(destructors)
	{
		destructors->disableNewUsages();
	}

	// Do this for every derived class since they also inherit the untrivial "new" overload.
	// TODO: In case some derived class reverts this, constructors and destructors can be generated for that.
	for(size_t i = 0; i < entity.getDerivedCount(); i++)
	{
		disableUntrivialNew(entity.getDerived(i));
	}
}

}
