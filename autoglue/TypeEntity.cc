#include <autoglue/TypeEntity.hh>

namespace ag
{

TypeEntity::TypeEntity(std::string_view name, Type type)
	: Entity(name), type(type)
{
}

TypeEntity::Type TypeEntity::getType()
{
	return type;
}

}
