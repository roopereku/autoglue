#include <gen/EnumEntryEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

EnumEntryEntity::EnumEntryEntity(std::string_view name)
	: Entity(name)
{
}

void EnumEntryEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateEnumEntry(*this);
}

const char* EnumEntryEntity::getTypeString()
{
	return "Enum entry";
}

}
