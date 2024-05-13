#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

namespace ag
{

FunctionGroupEntity::FunctionGroupEntity(std::string_view name, FunctionEntity::Type type)
	: Entity(Type::FunctionGroup, name), type(type)
{
}

void FunctionGroupEntity::addOverload(std::shared_ptr <FunctionEntity>&& overload)
{
	if(!findMatchingParameters(*overload))
	{
		overload->setOverloadIndex(children.size());
		addChild(std::move(overload));
	}
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
	return static_cast <FunctionEntity&> (*children[index]);
}

FunctionEntity::Type FunctionGroupEntity::getType()
{
	return type;
}

bool FunctionGroupEntity::hasName(std::string_view str)
{
	// If this function group contains constructors, add an alias for
	// them so that they can be more easily resolved.
	if(type == FunctionEntity::Type::Constructor && str == "Constructor")
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

const char* FunctionGroupEntity::getTypeString()
{
	return "Function group";
}

}
