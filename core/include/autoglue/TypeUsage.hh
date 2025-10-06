#ifndef AUTOGLUE_TYPE_USAGE_HH
#define AUTOGLUE_TYPE_USAGE_HH

#include <autoglue/TypeDefinition.hh>

namespace ag
{

class Class;
class Enum;

class TypeModifierHolder
{
public:
	bool isConst() const
	{
		return mConst;
	}

	bool isReference() const
	{
		return mReference;
	}

	void assignFrom(TypeModifierHolder& holder)
	{
		mConst = holder.mConst;
		mReference = holder.mReference;
	}

protected:
	TypeModifierHolder()
		: mConst(false), mReference(false)
	{
	}

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

private:
	TypeDefinition& mUsedType;
};

}

#endif
