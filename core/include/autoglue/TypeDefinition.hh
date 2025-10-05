#ifndef AUTOGLUE_TYPE_DEFINITION_HH
#define AUTOGLUE_TYPE_DEFINITION_HH

namespace ag
{

class TypeDefinition
{
public:
	enum class Type
	{
		Class,
		Enum,
		Callable,

		Character,
		Integer,
		String,
		Float
	};

	Type getType() const
	{
		return mType;
	}

protected:
	TypeDefinition(Type type)
		: mType(type)
	{
	}

private:
	Type mType;
};

}

#endif
