#include <autoglue/Entity.hh>

#include <cassert>
#include <iostream>

namespace ag
{

Entity::Entity(Type type, std::string_view name)
	: type(type), name(name)
{
}

void Entity::addChild(std::shared_ptr <Entity>&& child)
{
	assert(child);

	adoptEntity(*child);
	children.emplace_back(child);
}

bool Entity::isRoot()
{
	return !parent;
}

Entity& Entity::getParent() const
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

std::shared_ptr <EntityContext> Entity::getContext() const
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
		if(child->hasName(current))
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
	if(parent)
	{
		parent->use();
	}

	usages++;

	if(usages == 1)
	{
		onFirstUse();
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

Entity::Type Entity::getType()
{
	return type;
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

bool Entity::hasName(std::string_view str)
{
	return name == str;
}

void Entity::onFirstUse()
{
}

void Entity::adoptEntity(Entity& entity)
{
	entity.parent = this;
}

}
