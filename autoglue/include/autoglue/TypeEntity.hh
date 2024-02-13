#ifndef AUTOGLUE_TYPE_ENTITY_HH
#define AUTOGLUE_TYPE_ENTITY_HH

#include <autoglue/Entity.hh>

namespace ag
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
