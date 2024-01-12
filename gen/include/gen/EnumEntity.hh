#ifndef GEN_ENUM_ENTITY_HH
#define GEN_ENUM_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class EnumEntity : public Entity
{
public:
	EnumEntity(std::string_view name) : Entity(name)
	{
	}

	const char* getTypeString() override
	{
		return "Enum";
	}
};

}

#endif
