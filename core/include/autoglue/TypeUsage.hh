#ifndef AUTOGLUE_TYPE_USAGE_HH
#define AUTOGLUE_TYPE_USAGE_HH

#include <autoglue/TypeDefinition.hh>

#include <string_view>

namespace ag
{

class Class;
class Enum;

class TypeModifierHolder
{
public:
	TypeModifierHolder(bool isConst, bool isReference)
		: mConst(isConst), mReference(isReference)
	{
	}

	bool isConst() const
	{
		return mConst;
	}

	bool isReference() const
	{
		return mReference;
	}

protected:
	TypeModifierHolder(const TypeModifierHolder& other)
		: mConst(other.mConst), mReference(other.mReference)
	{
	}

	bool mConst;
	bool mReference;
};

class TypeUsage : public TypeModifierHolder
{
public:
	TypeUsage(TypeDefinition& usedType, const TypeModifierHolder& holder)
		: TypeModifierHolder(holder), mUsedType(usedType)
	{
	}

	const TypeDefinition& getUsedType() const
	{
		return mUsedType;
	}

	bool matchName(std::wstring_view name) const;

private:
	TypeDefinition& mUsedType;
};

}

#endif
