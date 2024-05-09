#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

namespace ag
{

FunctionGroupEntity::FunctionGroupEntity(std::string_view name)
	: Entity(name)
{
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

void FunctionGroupEntity::onGenerate(BindingGenerator& generator)
{
	for(auto child : children)
	{
		child->generate(generator);
	}
}

bool FunctionGroupEntity::interceptNewChild(Entity& newChild)
{
	// Ignore duplicate functions.
	auto& function = static_cast <FunctionEntity&> (newChild);
	return !findMatchingParameters(function);
}

const char* FunctionGroupEntity::getTypeString()
{
	return "Function group";
}

}
