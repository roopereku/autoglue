#ifndef GEN_CLASS_ENTITY_HH
#define GEN_CLASS_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class ClassEntity : public Entity
{
public:
	ClassEntity(std::string_view name) : Entity(name)
	{
	}

	const char* getTypeString() override
	{
		return "Class";
	}
};

}

#endif
