#ifndef AUTOGLUE_FIELD_HH
#define AUTOGLUE_FIELD_HH

#include <autoglue/Variable.hh>

namespace ag
{

class AbstractClass;

// Field defines a variable that's a member of a class.
class Field : public Variable
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Field;
	}

	Field(std::wstring&& name)
		: Variable(std::move(name), Node::Type::Field)
	{
	}
};

class AbstractField : public AbstractVariable
{
public:
	/// Gets the parent class.
	///
	/// \return The parent class.
	virtual const AbstractClass& getParentClass() const = 0;

	/// Gets the parent class.
	///
	/// \return The parent class.
	const AbstractNode& getParent() const final override;

protected:
	AbstractField(std::wstring&& name)
		: AbstractVariable(Node::Type::Field, std::move(name))
	{
	}
};

}

#endif
