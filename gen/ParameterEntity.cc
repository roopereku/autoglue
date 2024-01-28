#include <gen/ParameterEntity.hh>
#include <gen/BindingGenerator.hh>

#include <iostream>

namespace gen
{

ParameterEntity::ParameterEntity(std::string_view name, std::shared_ptr <ClassEntity> type)
	: Entity(name), type(type)
{
}

void ParameterEntity::generate(BindingGenerator& generator)
{
	generator.generateParameter(*this);
}

const char* ParameterEntity::getTypeString()
{
	return "Parameter";
}

void ParameterEntity::onList(std::string_view indent)
{
	Entity::onList(indent);

	if(type)
	{
		std::cout << indent << " -> " << type->getName() << '\n';
	}
}

}
