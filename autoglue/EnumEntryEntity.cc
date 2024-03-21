#include <autoglue/EnumEntryEntity.hh>
#include <autoglue/BindingGenerator.hh>

namespace ag
{

EnumEntryEntity::EnumEntryEntity(std::string_view name, std::string&& value, Format format)
	: Entity(name), value(std::move(value)), format(format)
{
}

void EnumEntryEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateEnumEntry(*this);
}

std::string_view EnumEntryEntity::getValue()
{
	return value;
}

EnumEntryEntity::Format EnumEntryEntity::getFormat()
{
	return format;
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
