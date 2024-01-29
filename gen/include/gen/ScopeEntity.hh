#ifndef GEN_SCOPE_ENTITY_HH
#define GEN_SCOPE_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class ScopeEntity : public Entity
{
public:
	ScopeEntity(std::string_view name);
	ScopeEntity();

	const char* getTypeString() override;

private:
	/// Generates the child entities with scope beginning and the ending if the name is set.
	void onGenerate(BindingGenerator& generator) override;
};

}

#endif
