#include <gen/EnumEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

EnumEntity::EnumEntity(std::string_view name)
	: Entity(name)
{
}

void EnumEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateEnumBeginning(*this);
	
	for(auto child : children)
	{
		child->generate(generator);
	}

	generator.generateEnumEnding(*this);
}

const char* EnumEntity::getTypeString()
{
	return "Enum";
}

}
