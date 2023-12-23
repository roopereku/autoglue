#include <gen/Entity.hh>

namespace gen
{

void Entity::addChild(std::shared_ptr <Entity>&& child)
{
	children.emplace_back(std::move(child));
}

}
