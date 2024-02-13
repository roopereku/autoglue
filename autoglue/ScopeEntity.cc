#include <autoglue/ScopeEntity.hh>
#include <autoglue/BindingGenerator.hh>

namespace ag
{

ScopeEntity::ScopeEntity(std::string_view name)
	: Entity(name)
{
}

ScopeEntity::ScopeEntity()
{
}

void ScopeEntity::onGenerate(BindingGenerator& generator)
{
	// If the scope has a name, generate a beginning for it.
	if(!name.empty())
	{
		generator.generateNamedScopeBeginning(*this);
	}

	// Generate all nested entities.
	for(auto child : children)
	{
		child->generate(generator);
	}

	// If the scope has a name, generate an ending for it.
	if(!name.empty())
	{
		generator.generateNamedScopeEnding(*this);
	}
}

const char* ScopeEntity::getTypeString() 
{
	return "Scope";
}

}
