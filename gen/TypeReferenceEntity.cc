#include <gen/TypeReferenceEntity.hh>
#include <gen/BindingGenerator.hh>

#include <iostream>

namespace gen
{

TypeReferenceEntity::TypeReferenceEntity(std::string_view name, std::shared_ptr <ClassEntity> type)
	: Entity(name), type(type)
{
}

void TypeReferenceEntity::generate(BindingGenerator& generator)
{
	generator.generateTypeReference(*this);
}

ClassEntity& TypeReferenceEntity::getType()
{
	return *type;
}

const char* TypeReferenceEntity::getTypeString()
{
	return "TypeReference";
}

void TypeReferenceEntity::onList(std::string_view indent)
{
	Entity::onList(indent);

	if(type)
	{
		std::cout << indent << " -> " << type->getName() << '\n';
	}
}

}
