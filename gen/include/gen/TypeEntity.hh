#ifndef GEN_TYPE_ENTITY_HH
#define GEN_TYPE_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class TypeEntity : public Entity
{
public:
	enum class Type
	{
		Primitive,
		Alias,
		Class,
		Enum
	};

	TypeEntity(std::string_view name, Type type);

	/// Gets the type of this entity.
	///
	/// \return The type of the this type entity.
	Type getType();

private:
	Type type;
};

}

#endif
