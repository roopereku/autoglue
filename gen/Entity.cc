#include <gen/Entity.hh>

#include <cassert>
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
	if(interceptNewChild(*child))
	{
		child->parent = this;
		children.emplace_back(std::move(child));
	}
}

bool Entity::isRoot()
{
	return !parent;
}

Entity& Entity::getParent()
{
	assert(parent);
	return *parent;
}

void Entity::initializeContext(std::shared_ptr <EntityContext>&& ctx)
{
	if(!context)
	{
		context = std::move(ctx);
	}
}

std::shared_ptr <EntityContext> Entity::getContext()
{
	return context;
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

std::string Entity::getHierarchy(const std::string& delimiter)
{
	// If this is the root entity, only return the name.
	if(isRoot())
	{
		return name;
	}

	// Don't add the delimiter if the parent name is empty.
	auto parentName = parent->getHierarchy(delimiter);
	if(parentName.empty())
	{
		return name;
	}

	// Combine the parent name and the name of this entity with a delimiter in between.
	return std::move(parentName) + delimiter + name;

}

void Entity::generate(BindingGenerator& generator)
{
	// Don't generate this entity if it already is generated or
	// if it is not used anywhere.
	if(generated || usages == 0)
	{
		return;
	}

	generated = true;
	onGenerate(generator);
}

void Entity::resetGenerated()
{
	generated = false;

	for(auto child : children)
	{
		child->resetGenerated();
	}
}

bool Entity::isGenerated()
{
	return generated;
}

void Entity::use()
{
	usages++;

	if(usages == 1)
	{
		onFirstUse();
	}

	if(parent)
	{
		parent->use();
	}
}

void Entity::useAll()
{
	use();

	for(auto child : children)
	{
		child->useAll();
	}
}

unsigned Entity::getUsages()
{
	return usages;
}

void Entity::list(unsigned depth)
{
	std::string indent(depth, '-');
	onList(indent);

	for(auto child : children)
	{
		child->list(depth + 1);
	}
}

void Entity::onList(std::string_view indent)
{
	std::cout << indent << " " << getTypeString() << " " << name << '\n';
}

void Entity::onFirstUse()
{
}

bool Entity::interceptNewChild(Entity&)
{
	return true;
}

}
