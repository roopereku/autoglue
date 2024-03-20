#include <autoglue/clang/GlueGenerator.hh>
#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/EntityContext.hh>

#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/FunctionEntity.hh>

#include <iostream>
#include <set>

namespace ag::clang
{

class IncludeCollector : public BindingGenerator
{
public:
	IncludeCollector(Backend& backend)
		: BindingGenerator(backend)
	{
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
			if(entity.getReferred().getContext())
			{
				auto ctx = std::static_pointer_cast <EntityContext> (entity.getReferred().getContext());
				includes.emplace("#include <" + ctx->getInclude() + ">");
			}
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
				<< "*> (thisPtr)";

			// Parameters aren't used for the destructor.
			return;
		}

		case FunctionEntity::Type::MemberFunction:
		{
			file << "static_cast <" << entity.getParent().getHierarchy("::")
				<< "*> (thisPtr)->" << entity.getName();
			break;
		}

		case FunctionEntity::Type::Function:
		{
			break;
		}
	}

	file << '(';
	onlyParameterNames = true;
	entity.generateParameters(*this, true, true);
	onlyParameterNames = false;
	file << ");\n";

	file << "}\n\n";
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		// First make the parameter a non-pointer lvalue.
		std::string lvalue;

		switch(entity.getType())
		{
			case TypeEntity::Type::Alias:
			case TypeEntity::Type::Class:
			{
				// Since class instances are void*, cast them to the appropriate pointer
				// type and dereference them to get an lvalue reference.
				lvalue = "*static_cast <" + entity.getReferred().getHierarchy("::") +
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
			
			default: {}
		}

		file << lvalue;
	}

	else
	{
		switch(entity.getType())
		{
			case TypeEntity::Type::Alias:
			case TypeEntity::Type::Class:
			{
				file << "void* " << entity.getName();
				break;
			}

			case TypeEntity::Type::Enum:
			{
				// TODO: Use the actual underlying type of an enum?
				file << "int " << entity.getName();
				break;
			}

			case TypeEntity::Type::Primitive:
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
				break;
			}
		}
	}
}

void GlueGenerator::generateArgumentSeparator()
{
	file << ", ";
}

}
