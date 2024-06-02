#include <autoglue/clang/GlueGenerator.hh>
#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/EntityContext.hh>
#include <autoglue/clang/TypeContext.hh>
#include <autoglue/clang/TyperefContext.hh>
#include <autoglue/clang/FunctionContext.hh>
#include <autoglue/clang/OverloadContext.hh>

#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/FunctionEntity.hh>

#include <iostream>
#include <set>

namespace ag::clang
{

static std::shared_ptr <EntityContext> getClangContext(const Entity& entity)
{
	if(entity.getContext())
	{
		return std::static_pointer_cast <EntityContext> (entity.getContext());
	}

	return nullptr;
}

static bool stringNeedsDuplication(FunctionEntity& entity)
{
	// If this function is a constructor for std::string_view, the passed in strings
	// should be duplicated in order for the string_view to be able to reference them.
	// This has to be done because things like C#, Java and Python will destroy
	// a passed in c-string as soon as they don't need it.
	//
	// This is a bit hacky because std::string_view is supposed to be a
	// non-owning type, but what can you really do?
	//
	// TODO: Similiar things may need to be done for other referencing types?
	assert(entity.getType() == FunctionEntity::Type::Constructor ||
			entity.getType() == FunctionEntity::Type::Destructor);

	auto ctx = getClangContext(entity.getParent());
	return ctx && ctx->getTypeContext()->getRealName() == "std::basic_string_view<char,std::char_traits<char>>";
}

static std::string getSelfType(FunctionEntity& entity)
{
	// If this function was originally a C++ function, the backend should've initialized
	// a typeref context for the function group. This context would contain the full
	// type (including templates) of the "this type" for the function.
	//
	// If the context isn't present, just use the location provided by the hierarchy.
	auto ctx = getClangContext(entity.getGroup());
	return ctx ? std::string(ctx->getFunctionContext()->getSelfType()) : entity.getParent().getHierarchy("::");

}

void generateTypePOD(std::ofstream& file, TypeReferenceEntity& entity)
{
	switch(entity.getPrimitiveType().getType())
	{
		case PrimitiveEntity::Type::String: file << "const char *"; break;
		case PrimitiveEntity::Type::ObjectHandle: file << "void*"; break;
		case PrimitiveEntity::Type::Character: file << "char"; break;
		case PrimitiveEntity::Type::Double: file << "double"; break;
		case PrimitiveEntity::Type::Boolean: file << "bool"; break;
		case PrimitiveEntity::Type::Float: file << "float"; break;
		case PrimitiveEntity::Type::Integer: file << "int"; break;
		case PrimitiveEntity::Type::Void: file << "void"; break;
	}
}

class IncludeCollector : public BindingGenerator
{
public:
	IncludeCollector(Backend& backend)
		: BindingGenerator(backend)
	{
	}

	void generateNamedScope(ScopeEntity& entity) override
	{
		entity.generateNested(*this);
	}

	void generateClass(ClassEntity& entity) override
	{
		auto ctx = getClangContext(entity);
		if(ctx)
		{
			addInclude(ctx->getTypeContext()->getInclude());
		}

		entity.generateNested(*this);
	}

	void generateFunction(FunctionEntity& entity) override
	{
		entity.generateReturnType(*this, true);
		entity.generateParameters(*this, true, true);
	}

	void generateTypeReference(TypeReferenceEntity& entity) override
	{
		if(!entity.isPrimitive())
		{
			auto ctx = getClangContext(entity.getReferred());
			if(ctx)
			{
				includes.emplace("#include <" + ctx->getTypeContext()->getInclude() + ">");
			}
		}
	}

	void addInclude(const std::string& include)
	{
		if(!include.empty())
		{
			includes.emplace("#include <" + include + ">");
		}
	}

	std::set <std::string> includes;
};

class ClassGenerator : public ag::BindingGenerator
{
public:
	ClassGenerator(Backend& backend, std::ofstream& file)
		: BindingGenerator(backend), file(file)
	{
	}

	void generateNamedScope(ScopeEntity& entity) override
	{
		file << "namespace AG_" << entity.getName() << "\n{\n";
		entity.generateNested(*this);
		file << "}\n";
	}

	void generateClass(ClassEntity& entity) override
	{
		if(!entity.isConcreteType())
		{
			file << "struct " << "AG_" << entity.getName();

			auto ctx = getClangContext(entity);
			if(ctx)
			{
				file << " : public " << ctx->getTypeContext()->getRealName();
			}

			file << "\n{\n";

			auto constructors = entity.resolve("Constructor");
			if(constructors)
			{
				inOverride = true;
				constructors->generate(*this);
				constructors->resetGenerationState(*this, false);
				inOverride = false;
			}

			entity.generateInterceptionContext(*this);
			entity.generateInterceptionFunctions(*this);

			entity.generateConcreteType(*this);
		}

		inBridge = true;
		entity.generateNested(*this);
		inBridge = false;

		if(!entity.isConcreteType())
		{
			file << "};\n\n";
		}
	}

	void generateFunction(FunctionEntity& entity) override
	{
		if(inOverride)
		{
			assert(entity.getType() == FunctionEntity::Type::Constructor);

			file << "AG_" + entity.getParent().getName() << '(';
			entity.generateParameters(*this, false, false);
			file << ')';

			auto baseCtx = getClangContext(entity.getParent());
			if(baseCtx)
			{
				file << " : " << baseCtx->getTypeContext()->getRealName() << '(';

				onlyParameterNames = true;
				entity.generateParameters(*this, false, false);
				onlyParameterNames = false;

				file << ')';
			}

			file << "\n{\n";
			file << "}\n";
		}

		else if(inBridge)
		{
			auto overridden = entity.getOverridden();
			if(overridden)
			{
				// This function should be an override residing in a concrete type.
				// The actual containing class is the parent of the concrete type.
				auto& containing = entity.getParent().getParent();

				assert(ClassEntity::matchType(overridden->getParent()));
				assert(ClassEntity::matchType(containing));

				// Only generate functions for a virtual call in the class that originally defined
				// the given overridable function.
				if(static_cast <ClassEntity&> (containing).shared_from_this() !=
					static_cast <ClassEntity&> (overridden->getParent()).shared_from_this())
				{
					return;
				}
			}

			else
			{
				auto ctx = getClangContext(entity);
				assert(ctx);

				// Don't generate in-class bridges for private interface overrides
				// as they simply call a virtual bridge function and don't need data access.
				if(ctx->getOverloadContext()->isPrivateOverride())
				{
					return;
				}
			}

			file << "static ";
			entity.generateReturnType(*this, true);

			if(overridden)
			{
				file << "AG_virtual_";
			}

			file << entity.getBridgeName(true) << '(';
			entity.generateParameters(*this, true, true);
			file << ")\n{\n    ";

			bool closeParenthesis = entity.generateReturnStatement(*this, false);
			entity.generateBridgeCall(*this);

			if(closeParenthesis)
			{
				file << ')';
			}

			file << ";\n}\n";
		}
	}

	void generateBridgeCall(FunctionEntity& entity) override
	{
		if(inIntercept)
		{
			file << "AG_intercept_" << entity.getBridgeName(true) << "(AG_foreignObject";
			if(entity.getParameterCount() > 0)
			{
				file << ',';
			}

			onlyParameterNames = true;
			entity.generateParameters(*this, false, false);
			onlyParameterNames = false;

			return;
		}

		switch(entity.getType())
		{
			case FunctionEntity::Type::MemberFunction:
			{
				auto overridden = entity.getOverridden();

				if(entity.getOverridden())
				{
					file << "static_cast <AG_" << overridden->getParent().getHierarchy("::AG_") << "*> (" <<
							getObjectHandleName() << ")->";
				}

				else
				{
					file << "static_cast <AG_" << entity.getParent().getHierarchy("::AG_") << "*> (" <<
							getObjectHandleName() << ")->" << getSelfType(entity) << "::";
				}

				file << entity.getName() << '(';

				onlyParameterNames = true;
				entity.generateParameters(*this, false, false);
				onlyParameterNames = false;

				file << ')';
				break;
			}

			case FunctionEntity::Type::Constructor:
			{
				file <<  "AG_" + entity.getParent().getHierarchy("::AG_") << '(';

				duplicateString = stringNeedsDuplication(entity);

				onlyParameterNames = true;
				castPrimitives = true;
				entity.generateParameters(*this, false, false);
				onlyParameterNames = false;
				castPrimitives = false;

				duplicateString = false;

				file << ')';
				break;
			}

			case FunctionEntity::Type::Destructor:
			{
				// If strdup was called for a std::string_view parameter, delete the duplicated string.
				if(stringNeedsDuplication(entity))
				{
					file << "free(const_cast <char*> (static_cast <AG_" << entity.getParent().getHierarchy("::AG_") <<
							"*> (" << getObjectHandleName() << ")->data()));\n";
				}

				file << "delete static_cast <AG_" << entity.getParent().getHierarchy("::AG_") <<
						"*> (" << getObjectHandleName() << ')';

				break;
			}

			case FunctionEntity::Type::Function:
			{
				// Nothing is generated in-class for a non-class method.
				break;
			}
		}
	}

	bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target) override
	{
		if(entity.isAlias())
		{
			TypeReferenceEntity underlying("", entity.getAliasType().getUnderlying(true), entity.isReference());
			underlying.initializeContext(entity.getContext());
			return generateReturnStatement(underlying, target);
		}

		if(inIntercept)
		{
			int toClose = 0;
			file << "return ";

			if(!entity.isReference())
			{
				// If the type to return isn't trivially copyable, let's move it instead and
				// return the object that way.
				// TODO: What if the type isn't moveable?
				auto ctx = getClangContext(entity);
				if(ctx && !ctx->getTyperefContext()->isTypeTriviallyCopyable())
				{
					file << "std::move(";
					toClose++;
				}
			}

			toClose += generateForeignToGlue(entity);

			target.generateBridgeCall(*this);

			while(toClose > 0)
			{
				file << ')';
				toClose--;
			}
		}

		else if(inBridge)
		{
			bool closeParenthesis = false;
			file << "return ";

			// If a non-reference class type is returned, return a heap allocated copy of it.
			if(!entity.isReference() && entity.getType() == TypeEntity::Type::Class)
			{
				// TODO: What if the "operator new" is protected and new is invoked
				// and this class has no access to it (Class of different type).
				file << "new ";

				// If this is not a constructor call, specify the type.
				if(target.getType() != FunctionEntity::Type::Constructor)
				{
					auto ctx = getClangContext(entity);
					assert(ctx);

					// TODO: What if a protected constructor is used?
					file << ctx->getTyperefContext()->getWrittenType() << '(';
					closeParenthesis = true;
				}
			}

			return generateGlueToForeign(entity) + closeParenthesis;
		}

		return false;
	}

	void generateTypeReference(TypeReferenceEntity& entity) override
	{
		// Type names and reference names.
		if(!onlyParameterNames)
		{
			if(inOverride)
			{
				auto ctx = getClangContext(entity);
				assert(ctx);

				file << ctx->getTyperefContext()->getOriginalType();
			}

			else if(inBridge)
			{
				assert(entity.isPrimitive());
				generateTypePOD(file, entity);
			}

			file << ' ' << entity.getName();
		}

		// Parameters passed to interception functions.
		else if(inIntercept)
		{
			int toClose = generateGlueToForeign(entity);
			file << entity.getName();

			while(toClose > 0)
			{
				file << ')';
				toClose--;
			}
		}

		// Parameters passed in override functions.
		else if(inOverride)
		{
			auto ctx = getClangContext(entity);
			assert(ctx);

			if(ctx->getTyperefContext()->isRValueReference())
			{
				file << "std::move(" << entity.getName() << ')';
			}	

			else
			{
				file << entity.getName();
			}
		}

		// Parameters passed in class bridge functions.
		else if(inBridge)
		{
			int toClose = generateForeignToGlue(entity);
			file << entity.getName();

			while(toClose > 0)
			{
				file << ')';
				toClose--;
			}
		}
	}

	void generateInterceptionFunction(FunctionEntity& entity, ClassEntity&) override
	{
		if(entity.getType() == FunctionEntity::Type::Destructor ||
			entity.getOverloadedOperator() != FunctionEntity::OverloadedOperator::None)
		{
			return;
		}

		inBridge = true;
		entity.generateReturnType(*this, true);
		file << "(*AG_intercept_" << entity.getBridgeName(true) << ")(";
		entity.generateParameters(*this, true, true);
		file << ");\n";
		inBridge = false;

		inOverride = true;
		entity.generateReturnType(*this, false);
		file << entity.getName() << '(';
		entity.generateParameters(*this, false, false);
		inOverride = false;

		file << ") " << getClangContext(entity)->getOverloadContext()->getEastQualifiers() << "override\n{\n";
		inIntercept = true;

		if(entity.returnsValue())
		{
			entity.generateReturnStatement(*this, false);
		}

		else
		{
			entity.generateBridgeCall(*this);
		}

		inIntercept = false;
		file << ");\n}\n";
	}

	void generateInterceptionContext(ClassEntity& entity) override
	{
		file << "void* AG_foreignObject;\n";
	}

	void generateArgumentSeparator() override
	{
		file << ", ";
	}

	std::string_view getObjectHandleName() override
	{
		return "objectHandle";
	}

	bool generateGlueToForeign(TypeReferenceEntity& entity)
	{
		switch(entity.getType())
		{
			case TypeEntity::Type::Class:
			{
				if(inIntercept || entity.isReference())
				{
					auto ctx = getClangContext(entity);
					assert(ctx);

					// If the returned reference object is not a pointer, take its address.
					if(!ctx->getTyperefContext()->isPointer())
					{
						// Take away the constness from const types.
						// TODO: Use something like const_cast instead?
						if(ctx->getTyperefContext()->isConst())
						{
							file << "(void*)";
						}

						file << '&';
					}
				}

				return false;
			}

			case TypeEntity::Type::Enum:
			{
				// TODO: Use whatever integral format the enum has specified.
				file << "static_cast <int> (";
				return true;
			}

			case TypeEntity::Type::Primitive:
			{
				return false;
			}

			case TypeEntity::Type::Alias:
			{
				TypeReferenceEntity underlying("", entity.getAliasType().getUnderlying(true), entity.isReference());
				underlying.initializeContext(entity.getContext());
				return generateGlueToForeign(underlying);
			}
		}
	}

	int generateForeignToGlue(TypeReferenceEntity& entity)
	{
		int toClose = 0;

		switch(entity.getType())
		{
			case TypeEntity::Type::Class:
			{
				auto ctx = getClangContext(entity);
				assert(ctx);

				if(ctx->getTyperefContext()->isRValueReference())
				{
					file << "std::move(";
					toClose++;
				}

				// If the class type parameter isn't a reference or a pointer,
				// dereference the casted pointer type.
				if(!entity.isReference() || !ctx->getTyperefContext()->isPointer())
				{
					file << '*';
				}

				file << "static_cast <" << ctx->getTyperefContext()->getWrittenType() << "*> (";
				toClose++;
				break;
			}

			case TypeEntity::Type::Enum:
			{
				auto ctx = getClangContext(entity);
				assert(ctx);

				file << "static_cast <" << ctx->getTyperefContext()->getWrittenType() << "> (";
				toClose++;

				break;
			}

			case TypeEntity::Type::Primitive:
			{
				// If a string should be duplicated, pass it into stdup.
				if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::String && duplicateString)
				{
					file << "strdup(";
					toClose++;
				}

				if(castPrimitives)
				{
					auto ctx = getClangContext(entity);
					assert(ctx);

					file << "static_cast <" << ctx->getTyperefContext()->getOriginalType() << "> (";
					toClose++;
				}

				break;
			}

			case TypeEntity::Type::Alias:
			{
				TypeReferenceEntity underlying(entity.getName(), entity.getAliasType().getUnderlying(true), entity.isReference());
				underlying.initializeContext(entity.getContext());
				return generateForeignToGlue(underlying);
			}
		}

		return toClose;
	}

private:
	std::ofstream& file;

	bool inBridge = false;
	bool inOverride = false;
	bool inIntercept = false;
	bool onlyParameterNames = false;
	bool castPrimitives = false;
	bool duplicateString = false;
};

GlueGenerator::GlueGenerator(Backend& backend)
	: BindingGenerator(backend), file("glue.cpp")
{
	IncludeCollector collector(backend);
	collector.generateBindings(false);

	for(auto& include : collector.includes)
	{
		file << include << '\n';
	}

	ClassGenerator classGen(backend, file);
	classGen.generateBindings(false);
}

void GlueGenerator::generateFunction(FunctionEntity& entity)
{
	if(entity.isInterface())
	{
		return;
	}

	file << "extern \"C\"\n";
	entity.generateReturnType(*this, true);
	file << entity.getBridgeName() << "(";

	entity.generateParameters(*this, true, true);
	file << ")\n{\n";

	entity.generateReturnStatement(*this, true);
	entity.generateBridgeCall(*this);

	file << ";\n}\n\n";
}

void GlueGenerator::generateNamedScope(ScopeEntity& entity)
{
	entity.generateNested(*this);
}

void GlueGenerator::generateClass(ClassEntity& entity)
{
	file << "// ---------- Class " << entity.getHierarchy("::") << " : " << " ----------\n\n";

	entity.generateInterceptionContext(*this);
	entity.generateNested(*this);
	entity.generateConcreteType(*this);
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		file << entity.getName();
	}

	else
	{
		generateTypePOD(file, entity);
		file << ' ' << entity.getName();
	}
}

void GlueGenerator::generateArgumentSeparator()
{
	file << ", ";
}

std::string_view GlueGenerator::getObjectHandleName()
{
	return "objectHandle";
}

bool GlueGenerator::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
	file << "return ";
	return false;
}

void GlueGenerator::generateBridgeCall(FunctionEntity& target)
{
	switch(target.getType())
	{
		case FunctionEntity::Type::MemberFunction:
		case FunctionEntity::Type::Constructor:
		case FunctionEntity::Type::Destructor:
		{
			// If the function represents an override within a concrete type, do a virtual
			// call of the original overridable function. This is because a foreign language
			// might not know the actual type that a concrete type represents.
			auto overridden = target.getOverridden();
			if(overridden)
			{
				file << "AG_" << overridden->getParent().getHierarchy("::AG_") << "::AG_virtual_" <<
						target.getBridgeName(true) << '(';
			}

			else
			{
				auto ctx = getClangContext(target);
				assert(ctx);

				// If the function is a private override of an interface, it cannot be called
				// without a virtual function call. Call the virtual function for interface instead.
				if(ctx->getOverloadContext()->isPrivateOverride())
				{
					auto& containing = ctx->getOverloadContext()->getOverriddenInterface()->getParent();
					file << "AG_" << containing.getHierarchy("::AG_") << "::AG_virtual_" <<
							target.getBridgeName(true) << '(';
				}

				// Call the corresponding function from the appropriate class.
				// While some functions could directly be called in these bridge functions,
				// stuff like protected functions cannot be invoked here directly. This
				// is why we are invoking a method from a generated class
				// impersonating the original class.
				else
				{
					file << "AG_" << target.getParent().getHierarchy("::AG_") << "::" <<
							target.getBridgeName(true) << '(';
				}
			}

			onlyParameterNames = true;
			target.generateParameters(*this, true, true);
			onlyParameterNames = false;

			file << ')';
			break;
		}

		case FunctionEntity::Type::Function:
		{
			// TODO: Implement non-class methods.
			break;
		}
	}
}

void GlueGenerator::generateInterceptionFunction(FunctionEntity& target, ClassEntity&)
{
	// Generate assignments for every interception function.
	if(onlyParameterNames)
	{
		std::string funcName("AG_intercept_" + target.getBridgeName(true));
		file << "obj->" << funcName << " = " << funcName << ";\n";
	}

	// Generate the function pointer parameters for the interception context
	// initialization function.
	else
	{
		file << ", ";

		target.generateReturnType(*this, true);
		file << "(*AG_intercept_" << target.getBridgeName(true) << ")(";
		target.generateParameters(*this, true, true);
		file << ')';
	}
}

void GlueGenerator::generateInterceptionContext(ClassEntity& entity)
{
	file << "extern \"C\"\n";
	file << "void " << entity.getHierarchy() << "_AG_initializeInterceptionContext(void* objectHandle, void* AG_foreignObject";
	entity.generateInterceptionFunctions(*this);

	file << ")\n{\n";
	file << "auto* obj = static_cast <AG_" << entity.getHierarchy("::AG_") << "*> (objectHandle);\n";
	file << "obj->AG_foreignObject = AG_foreignObject;";

	onlyParameterNames = true;
	entity.generateInterceptionFunctions(*this);
	onlyParameterNames = false;

	file << "}\n";
}

}
