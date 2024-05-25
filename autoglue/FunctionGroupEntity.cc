#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>

namespace ag
{

FunctionGroupEntity::FunctionGroupEntity(std::string_view name, FunctionEntity::Type type)
	: Entity(Type::FunctionGroup, name), type(type)
{
}

bool FunctionGroupEntity::addOverload(std::shared_ptr <FunctionEntity>&& overload)
{
	if(!findMatchingParameters(*overload))
	{
		if(overload->isInterface())
		{
			interfaces++;
		}

		if(overload->isOverridable())
		{
			overridables++;
		}

		overload->setOverloadIndex(children.size());
		addChild(std::move(overload));

		return true;
	}

	return false;
}

std::shared_ptr <FunctionEntity> FunctionGroupEntity::findMatchingParameters(FunctionEntity& entity)
{
	for(auto child : children)
	{
		auto& currentFunction = static_cast <FunctionEntity&> (*child);

		// If the current function has the same amount of parameters as
		// the given function, it could be a match.
		if(entity.getParameterCount() == currentFunction.getParameterCount())
		{
			size_t matched = 0;
			for(size_t i = 0; i < entity.getParameterCount(); i++)
			{
				if(entity.getParameter(i).isIdentical(currentFunction.getParameter(i)))
				{
					matched++;
				}
			}

			// If all of the parameters matched, return the current function.
			if(matched == entity.getParameterCount())
			{
				return std::static_pointer_cast <FunctionEntity> (child);
			}
		}
	}

	return nullptr;
}

size_t FunctionGroupEntity::getOverloadCount()
{
	return children.size();
}

FunctionEntity& FunctionGroupEntity::getOverload(size_t index)
{
	assert(children[index]->getType() == Entity::Type::Function);
	return static_cast <FunctionEntity&> (*children[index]);
}

FunctionEntity::Type FunctionGroupEntity::getType()
{
	return type;
}

bool FunctionGroupEntity::hasOverridable()
{
	return overridables > 0;
}

bool FunctionGroupEntity::hasName(std::string_view str)
{
	// If this function group contains constructors, add an alias for
	// them so that they can be more easily resolved.
	if(type == FunctionEntity::Type::Constructor && str == "Constructor")
	{
		return true;
	}

	// If this function group contains destructors, add an alias for
	// them so that they can be more easily resolved.
	else if(type == FunctionEntity::Type::Destructor && str == "Destructor")
	{
		return true;
	}

	return Entity::hasName(str);
}

void FunctionGroupEntity::onGenerate(BindingGenerator& generator)
{
	for(auto child : children)
	{
		child->generate(generator);
	}
}

void FunctionGroupEntity::onFirstUse()
{
	for(auto child : children)
	{
		child->use();
	}
}

const char* FunctionGroupEntity::getTypeString()
{
	return "Function group";
}

}
