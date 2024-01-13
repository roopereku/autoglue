#include <gen/Entity.hh>

#include <iostream>

namespace gen
{

Entity::Entity(std::string_view name) : name(name)
{
}

Entity::Entity()
{
}

void Entity::addChild(std::shared_ptr <Entity>&& child)
{
	children.emplace_back(std::move(child));
}

std::shared_ptr <Entity> Entity::resolve(std::string_view qualifiedName)
{
	// Try to find the first dot. if there is none, stop at the string end.
	size_t nextDot = qualifiedName.find('.');
	size_t currentEnd = nextDot == std::string_view::npos ? qualifiedName.size() : nextDot;

	// Get the name preceding the first dot.
	std::string_view current(qualifiedName.substr(0, currentEnd));

	for(auto child : children)
	{
		if(child->name == current)
		{
			// If there are no more dots, this is the resulting entity.
			if(currentEnd == qualifiedName.size())
			{
				return child;
			}

			// Recursively call resolve without the current name and the dot.
			return child->resolve(qualifiedName.substr(currentEnd + 1, qualifiedName.size()));
		}
	}

	return nullptr;
}

const std::string& Entity::getName() const
{
	return name;
}

void Entity::list(unsigned depth)
{
	std::string indent(depth, '-');
	std::cout << indent << " " << getTypeString() << " " << name << '\n';

	for(auto child : children)
	{
		child->list(depth + 1);
	}
}

}
