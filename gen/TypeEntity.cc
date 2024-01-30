#include <gen/TypeEntity.hh>

namespace gen
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
