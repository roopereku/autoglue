#include <autoglue/FunctionEntity.hh>
#include <autoglue/BindingGenerator.hh>
#include <autoglue/TypeReferenceEntity.hh>

namespace ag
{

FunctionEntity::FunctionEntity(std::string_view name, Type type)
	: Entity(name), type(type)
{
}

FunctionEntity::FunctionEntity(std::string_view name, Type type,
								std::shared_ptr <TypeReferenceEntity>&& returnType)
	: Entity(name), type(type), returnType(std::move(returnType))
{
}

void FunctionEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateFunction(*this);
}

void FunctionEntity::onFirstUse()
{
	if(returnType)
	{
		returnType->use();
	}
}

void FunctionEntity::generateReturnType(BindingGenerator& generator)
{
	if(returnType)
	{
		generator.generateTypeReference(*returnType);
	}
}

void FunctionEntity::generateParameters(BindingGenerator& generator)
{
	for(size_t i = 0; i < children.size(); i++)
	{
		// Generate the parameter. It is represented as a named type reference.
		generator.generateTypeReference(static_cast <TypeReferenceEntity&> (*children[i]));

		// Add an argument separator for parameters that aren't the last one.
		if(i != children.size() - 1)
		{
			generator.generateArgumentSeparator();
		}
	}
}

size_t FunctionEntity::getParameterCount()
{
	return children.size();
}

TypeReferenceEntity& FunctionEntity::getParameter(size_t index)
{
	return static_cast <TypeReferenceEntity&> (*children[index]);
}

FunctionEntity::Type FunctionEntity::getType()
{
	return type;
}

bool FunctionEntity::needsThisHandle()
{
	// TODO: Exclude static functions.
	return type == Type::MemberFunction ||
			type == Type::Destructor;
}

bool FunctionEntity::returnsValue()
{
	// TODO: Don't assume "void" but add an abstract flag.
	// That way any backend can decide whether a type returns a value.
	return type == Type::Constructor ||
			(returnType && returnType->getReferred().getName() != "void");
}

const char* FunctionEntity::getTypeString()
{
	return "Function";
}

}
