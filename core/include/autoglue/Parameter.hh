#ifndef AUTOGLUE_PARAMETER_HH
#define AUTOGLUE_PARAMETER_HH

#include <autoglue/Variable.hh>
#include <autoglue/TypeUsage.hh>

namespace ag
{

class AbstractFunction;

class Parameter : public Variable
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Parameter;
	}

	Parameter(std::string_view name)
		: Variable(name, Node::Type::Parameter)
	{
	}
};

class AbstractParameter : public AbstractVariable
{
public:
	/// Gets the parent function.
	///
	/// \return The parent function.
	virtual const AbstractFunction& getParentFunction() const = 0;

	/// Gets the parent function.
	///
	/// \return The parent function.
	const AbstractNode& getParent() const final override;

protected:
	AbstractParameter(std::string_view name)
		: AbstractVariable(Node::Type::Parameter, name)
	{
	}
};

}

#endif
