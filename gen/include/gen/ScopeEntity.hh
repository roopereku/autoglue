#ifndef GEN_SCOPE_ENTITY_HH
#define GEN_SCOPE_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class ScopeEntity : public Entity
{
public:
	ScopeEntity(std::string_view name) : Entity(name)
	{
	}

	ScopeEntity()
	{
	}

	const char* getTypeString() override
	{
		return "Scope";
	}
};

}

#endif
