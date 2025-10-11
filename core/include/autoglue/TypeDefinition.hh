#ifndef AUTOGLUE_TYPE_DEFINITION_HH
#define AUTOGLUE_TYPE_DEFINITION_HH

#include <memory>

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
		Float,

		Void
	};

	TypeDefinition(Type type)
		: mType(type)
	{
	}

	Type getType() const
	{
		return mType;
	}

	virtual bool matches(const TypeDefinition& other) const;
	virtual std::shared_ptr <TypeDefinition> copyToHeap() const;

private:
	Type mType;
};

}

#endif
