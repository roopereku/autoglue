#ifndef AUTOGLUE_SCOPE_ENTITY_HH
#define AUTOGLUE_SCOPE_ENTITY_HH

#include <autoglue/Entity.hh>

namespace ag
{

class ScopeEntity : public Entity
{
public:
	ScopeEntity(std::string_view name);
	ScopeEntity();

	/// Generates the nested entities within this scope.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateNested(BindingGenerator& generator);

	const char* getTypeString() override;

private:
	/// Generates the child entities with scope beginning and the ending if the name is set.
	void onGenerate(BindingGenerator& generator) override;
};

}

#endif
