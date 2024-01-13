#ifndef GEN_FUNCTION_ENTITY_HH
#define GEN_FUNCTION_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class FunctionEntity : public Entity
{
public:
	FunctionEntity(std::string_view name) : Entity(name)
	{
	}

	// TODO: Add return type.

	const char* getTypeString() override
	{
		return "Function";
	}
};

}

#endif
