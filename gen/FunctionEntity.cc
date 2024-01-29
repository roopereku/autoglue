#include <gen/FunctionEntity.hh>
#include <gen/BindingGenerator.hh>
#include <gen/TypeReferenceEntity.hh>

namespace gen
{

FunctionEntity::FunctionEntity(std::string_view name, Type type)
	: Entity(name), type(type)
{
}

void FunctionEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateFunction(*this);
}

void FunctionEntity::generateReturnType(BindingGenerator& generator)
{
	// TODO: Once return type exists, generate it.
}

void FunctionEntity::generateParameters(BindingGenerator& generator)
{
	// TODO: Adjust the offset when return type exists.
	for(size_t i = 0; i < children.size(); i++)
	{
		// Generate the parameter. It is represented as a named type reference.
		generator.generateTypeReference(static_cast <TypeReferenceEntity&> (*children[i]));

		// Add an argument separator for parameters that aren't the last one.
		if(i + 1 < children.size())
		{
			generator.generateArgumentSeparator();
		}
	}
}

size_t FunctionEntity::getParameterCount()
{
	// TODO: Account for the return type.
	return children.size();
}

//FunctionEntity::Type FunctionEntity::getType()
//{
//	return type;
//}
//
bool FunctionEntity::needsThisHandle()
{
	return true;
}
//	// TODO: Exclude static functions.
//	return type == Type::MemberFunction ||
//			type == Type::Destructor;
//}

const char* FunctionEntity::getTypeString()
{
	return "Function";
}

}
