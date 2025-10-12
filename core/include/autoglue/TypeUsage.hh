#ifndef AUTOGLUE_TYPE_USAGE_HH
#define AUTOGLUE_TYPE_USAGE_HH

#include <autoglue/TypeDefinition.hh>
#include <autoglue/Integer.hh>

#include <string_view>

namespace ag
{

class AbstractClass;
class AbstractEnum;

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

/// AbstractTypeUsage is used by source language specific tree building
/// code to provide a temporary source of information about which type is used and how it's being used.
class AbstractTypeUsage : public TypeModifierHolder
{
public:
	/// Gets the type of the used type definition.
	///
	/// \return The type of used type definition.
	TypeDefinition::Type getTypeOfUsedDefinition() const
	{
		return mUsedType;
	}

	/// Gets a class abstraction representing the declaration of the used type definition.
	/// Note: This is only called when a the type of used is a class.
	///
	/// \return The class abstraction of the used type definition.
	virtual const AbstractClass& getClass() const = 0;

	/// Gets an enum abstraction representing the declaration of the used type definition.
	/// Note: This is only called when a the type of used is a enum.
	///
	/// \return The enum abstraction of the used type definition.
	virtual const AbstractEnum& getEnum() const = 0;

	/// Gets the definition of what kind of an integer is being used.
	/// Note: This is only called when the reported type is an integer.
	///
	/// \return Definition describing the integer type that's being used.
	virtual const Integer getIntegerDefinition() const = 0;

protected:
	AbstractTypeUsage(TypeDefinition::Type usedType)
		: TypeModifierHolder(false, false), mUsedType(usedType)
	{
	}

private:
	TypeDefinition::Type mUsedType;
};

}

#endif
