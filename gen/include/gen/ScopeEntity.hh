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

	/// Generates this scope if the name is set.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator) override;

	const char* getTypeString() override;
};

}

#endif
