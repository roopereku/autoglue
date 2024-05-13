#include <autoglue/ScopeEntity.hh>
#include <autoglue/BindingGenerator.hh>

namespace ag
{

ScopeEntity::ScopeEntity(std::string_view name)
	: Entity(Type::Scope, name)
{
}

ScopeEntity::ScopeEntity()
	: Entity(Type::Scope, "")
{
}

void ScopeEntity::generateNested(BindingGenerator& generator)
{
	// Generate the nested entities.
	for(auto child : children)
	{
		child->generate(generator);
	}
}

void ScopeEntity::onGenerate(BindingGenerator& generator)
{
	// If this scope has a name, generate it with custom logic.
	if(!name.empty())
	{
		generator.generateNamedScope(*this);
	}

	// For unnamed scopes, implicitly generate the nested entities.
	// For example, the global scope is handled like this.
	else
	{
		generateNested(generator);
	}
}

const char* ScopeEntity::getTypeString() 
{
	return "Scope";
}

}
