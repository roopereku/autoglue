#include <gen/EnumEntryEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

EnumEntryEntity::EnumEntryEntity(std::string_view name, size_t value)
	: Entity(name), value(value)
{
}

void EnumEntryEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateEnumEntry(*this);
}

size_t EnumEntryEntity::getValue()
{
	return value;
}

bool EnumEntryEntity::isLast()
{
	return last;
}

const char* EnumEntryEntity::getTypeString()
{
	return "Enum entry";
}

}
