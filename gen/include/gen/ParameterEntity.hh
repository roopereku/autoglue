#ifndef GEN_PARAMETER_ENTITY_HH
#define GEN_PARAMETER_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class ParameterEntity : public Entity
{
public:
	ParameterEntity(std::string_view name) : Entity(name)
	{
	}

	// TODO: Add type.

	const char* getTypeString() override
	{
		return "Parameter";
	}
};

}

#endif
