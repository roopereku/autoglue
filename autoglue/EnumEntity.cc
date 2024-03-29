#include <autoglue/EnumEntity.hh>
#include <autoglue/EnumEntryEntity.hh>
#include <autoglue/BindingGenerator.hh>

namespace ag
{

EnumEntity::EnumEntity(std::string_view name)
	: TypeEntity(name, Type::Enum)
{
}

void EnumEntity::generateValues(BindingGenerator& generator)
{
	// Generate the enum values.
	for(auto child : children)
	{
		child->generate(generator);
	}
}

void EnumEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateEnum(*this);
}

void EnumEntity::onFirstUse()
{
	// If there are children, tell the most recent one that it's the last.
	if(!children.empty())
	{
		std::static_pointer_cast <EnumEntryEntity> (children.back())->last = true;
	}
}

const char* EnumEntity::getTypeString()
{
	return "Enum";
}

}
