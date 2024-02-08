#include <gen/FunctionGroupEntity.hh>
#include <gen/TypeReferenceEntity.hh>

namespace gen
{

FunctionGroupEntity::FunctionGroupEntity(std::string_view name)
	: Entity(name)
{
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
	auto& function = static_cast <FunctionEntity&> (newChild);

	for(auto child : children)
	{
		auto& currentFunction = static_cast <FunctionEntity&> (*child);

		if(function.getParameterCount() == currentFunction.getParameterCount())
		{
			size_t matched = 0;
			for(size_t i = 0; i < function.getParameterCount(); i++)
			{
				if(function.getParameter(i).isIdentical(currentFunction.getParameter(i)))
				{
					matched++;
				}
			}

			// TODO: Do soemthing with return value?

			if(matched == function.getParameterCount())
			{
				return false;
			}
		}
	}

	return true;
}

const char* FunctionGroupEntity::getTypeString()
{
	return "Function group";
}

}
