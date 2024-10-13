#include <autoglue/EnumEntryEntity.hh>
#include <autoglue/BindingGenerator.hh>

namespace ag
{

EnumEntryEntity::EnumEntryEntity(std::string_view name, std::string&& value)
	: Entity(Type::EnumEntry, name), value(std::move(value))
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

bool EnumEntryEntity::isLast()
{
	return last;
}

const char* EnumEntryEntity::getTypeString()
{
	return "EnumEntry";
}

}
