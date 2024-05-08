#include <autoglue/clang/GlueGenerator.hh>
#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/EntityContext.hh>
#include <autoglue/clang/IncludeContext.hh>
#include <autoglue/clang/TyperefContext.hh>

#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/FunctionEntity.hh>

#include <iostream>
#include <set>

namespace ag::clang
{

static std::shared_ptr <EntityContext> getClangContext(Entity& entity)
{
	if(entity.getContext())
	{
		return std::static_pointer_cast <EntityContext> (entity.getContext());
	}

	return nullptr;
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
			addInclude(ctx->getIncludeContext()->getInclude());
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
				includes.emplace("#include <" + ctx->getIncludeContext()->getInclude() + ">");
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

GlueGenerator::GlueGenerator(Backend& backend)
	: BindingGenerator(backend), file("glue.cpp")
{
	IncludeCollector collector(backend);
	collector.generateBindings();

	for(auto& include : collector.includes)
	{
		file << include << '\n';
	}
}

bool GlueGenerator::convertReturnIfNecessary(TypeReferenceEntity& entity)
{
	if(entity.isReference())
	{
		assert(entity.getContext());
		auto ctx = std::static_pointer_cast <EntityContext> (entity.getContext())->getTyperefContext();

		// If the reference return value isn't a pointer (Should be a C++ reference),
		// take its memory address.
		if(!ctx->isPointer())
		{
			file << "&";
		}

		return false;
	}

	// Primitive types don't need to be casted.
	if(entity.isPrimitive())
	{
		return false;
	}

	// Cast enum return values to an int. Especially in C++ this is important
	// because scoped enums cannot be implicitly casted to a primitive.
	if(entity.isEnum())
	{
		// TODO: Use the appropriate type as specified by the enum.
		file << "static_cast <int> (";
	}

	else if(entity.isClass())
	{
		assert(entity.getContext());

		// When a C++ function return a copy of a class object, in order to retain
		// the object it has to be allocated on the heap. Any given foreign language
		// should manage this object as they are the ones requesting it.
		auto ctx = std::static_pointer_cast <EntityContext> (entity.getContext());
		file << "new " << ctx->getTyperefContext()->getWrittenType() << '(';
	}

	else if(entity.isAlias())
	{
		// Because a type alias can point to whatever type, call this function recursively
		// with a temporary type reference pointing to the underlying type.
		TypeReferenceEntity underlying(entity.getName(), entity.getAliasType().getUnderlying(true), entity.isReference());
		underlying.initializeContext(entity.getContext());

		assert(underlying.getContext());
		return convertReturnIfNecessary(underlying);
	}

	return true;
}

void GlueGenerator::generateFunction(FunctionEntity& entity)
{
	file << "extern \"C\"\n";

	// In case the return type of a function is const, mark the bridge function
	// const to make C++ happy. This shouldn't affect foreign languages in any way.
	if(getClangContext(entity.getReturnType())->getTyperefContext()->isConst())
	{
		file << "const ";
	}

	entity.generateReturnType(*this, true);
	file << entity.getHierarchy() << "(";

	entity.generateParameters(*this, true, true);
	file << ")\n{\n";

	bool closeParenthesis = false;

	if(entity.returnsValue())
	{
		auto& ret = entity.getReturnType();
		file << "return ";

		closeParenthesis = convertReturnIfNecessary(ret);
	}

	switch(entity.getType())
	{
		case FunctionEntity::Type::Constructor:
		{
			file << "new " << entity.getParent().getHierarchy("::");
			break;
		}

		case FunctionEntity::Type::Destructor:
		{
			file << "delete static_cast <" << entity.getParent().getHierarchy("::")
				<< "*> (objectHandle)";

			// Parameters aren't used for destructors.
			return;
		}

		case FunctionEntity::Type::MemberFunction:
		{
			file << "static_cast <" << entity.getParent().getHierarchy("::")
				<< "*> (objectHandle)->" << entity.getName();
			break;
		}

		case FunctionEntity::Type::Function:
		{
			break;
		}
	}

	file << '(';
	onlyParameterNames = true;
	entity.generateParameters(*this, false, false);
	onlyParameterNames = false;
	file << ")";

	if(closeParenthesis)
	{
		file << ')';
	}

	file << ";\n}\n\n";
}

void GlueGenerator::generateNamedScope(ScopeEntity& entity)
{
	entity.generateNested(*this);
}

void GlueGenerator::generateClass(ClassEntity& entity)
{
	file << "// ---------- Class " << entity.getHierarchy("::") << " ----------\n\n";
	entity.generateNested(*this);
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		//std::cerr << "Passing param " << entity.getName() << ": " << entity.getReferred().getHierarchy() << " -> " << entity.getReferred().getTypeString() << '\n';

		// First make the parameter a non-pointer lvalue.
		std::string lvalue;
		auto ctx = getClangContext(entity);

		switch(entity.getType())
		{
			case TypeEntity::Type::Alias:
			{
				assert(ctx);

				// Because a type alias can point to any sort of type, call this function
				// recursively with the underlying type. Doing so will write the underlying type
				// in the appropriate way depending on what the type is.
				// In case the context is needed, copy it to the new type reference.
				TypeReferenceEntity underlying(entity.getName(), entity.getAliasType().getUnderlying(true), entity.isReference());
				underlying.initializeContext(std::move(ctx));
				generateTypeReference(underlying);

				break;
			}

			case TypeEntity::Type::Class:
			{
				assert(ctx);

				// Since class instances are void*, cast them to the appropriate pointer
				// type and dereference them to get an lvalue reference.
				lvalue = "*static_cast <" + ctx->getTyperefContext()->getWrittenType() +
						 "*> (" + entity.getName() + ')';
				break;
			}

			case TypeEntity::Type::Enum:
			{
				// Cast enum types from integers to the appropriate enum type.
				lvalue = "static_cast <" + entity.getReferred().getHierarchy("::") +
						 "> (" + entity.getName() + ')';
				break;
			}
			
			default:
			{
				lvalue = entity.getName();
				break;
			}
		}

		if(ctx && ctx->getTyperefContext()->isRValueReference())
		{
			// TODO: Include <utility> in IncludeCollector.
			file << "std::move(" << lvalue << ")";
		}

		else
		{
			file << lvalue;
		}
	}

	else
	{

		const char* typeName = "";

		if(entity.isReference())
		{
			typeName = "void*";
		}

		else
		{
			switch(entity.getPrimitiveType().getType())
			{
				case PrimitiveEntity::Type::String: typeName = "const char *"; break;
				case PrimitiveEntity::Type::ObjectHandle: typeName = "void*"; break;
				case PrimitiveEntity::Type::Character: typeName = "char"; break;
				case PrimitiveEntity::Type::Double: typeName = "double"; break;
				case PrimitiveEntity::Type::Boolean: typeName = "bool"; break;
				case PrimitiveEntity::Type::Float: typeName = "float"; break;
				case PrimitiveEntity::Type::Integer: typeName = "int"; break;
				case PrimitiveEntity::Type::Void: typeName = "void"; break;
			}
		}

		file << typeName << ' ' << entity.getName();
	}
}

void GlueGenerator::generateArgumentSeparator()
{
	file << ", ";
}

}
