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

void GlueGenerator::generateFunction(FunctionEntity& entity)
{
	file << "extern \"C\"\n";
	entity.generateReturnType(*this, true);
	file << entity.getHierarchy() << "(";

	entity.generateParameters(*this, true, true);
	file << ")\n{\n";

	switch(entity.getType())
	{
		case FunctionEntity::Type::Constructor:
		{
			file << "return new " << entity.getParent().getHierarchy("::");
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
	file << ");\n";

	file << "}\n\n";
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

		switch(entity.getType())
		{
			case TypeEntity::Type::Alias:
			{
				auto ctx = getClangContext(entity);
				assert(ctx);

				// Because a type alias can point to any sort of type, call this function
				// recursively with the underlying type. Doing so will write the underlying type
				// in the appropriate way depending on what the type is.
				// In case the context is needed, copy it to the new type reference.
				TypeReferenceEntity underlying(entity.getName(), entity.getAliasType().getUnderlying(true));
				underlying.initializeContext(std::move(ctx));
				generateTypeReference(underlying);

				break;
			}

			case TypeEntity::Type::Class:
			{
				auto ctx = getClangContext(entity);
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

		// TODO: Some conversion might need to be done for the lvalue in the future.
		file << lvalue;
	}

	else
	{
		const char* typeName = "";

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

		file << typeName << ' ' << entity.getName();
	}
}

void GlueGenerator::generateArgumentSeparator()
{
	file << ", ";
}

}
