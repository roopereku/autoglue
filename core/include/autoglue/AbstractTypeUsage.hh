#ifndef AUTOGLUE_ABSTRACT_TYPE_USAGE_HH
#define AUTOGLUE_ABSTRACT_TYPE_USAGE_HH

#include <autoglue/Node.hh>
#include <autoglue/TypeUsage.hh>
#include <autoglue/Integer.hh>

namespace ag
{

class AbstractNode;

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

	/// Gets a node abstraction representing the declaration of the used type definition.
	/// Note: This is only called when a type that is also a node is reported.
	///
	/// \return The node abstraction of the used type definition.
	virtual AbstractNode& getDeclarationOfUsed() const = 0;

	/// Gets the definition of what kind of an integer is being used.
	/// Note: This is only called when the reported type is an integer.
	///
	/// \return Definition describing the integer type that's being used.
	virtual Integer getIntegerDefinition() const = 0;

protected:
	AbstractTypeUsage(TypeDefinition::Type usedType)
		: mUsedType(usedType)
	{
	}

private:
	TypeDefinition::Type mUsedType;
};

}


#endif
