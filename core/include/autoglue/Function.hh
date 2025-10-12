#ifndef AUTOGLUE_FUNCTION_HH
#define AUTOGLUE_FUNCTION_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/SignatureHolder.hh>
#include <autoglue/Parameter.hh>

namespace ag
{

class Function : public Node, public SignatureHolder
{
public:
	static constexpr bool matchType(Node::Type type)
	{
		return type == Node::Type::Function;
	}

	Function(std::wstring_view name);

	/// Checks if the given name matches the function name and parameters.
	///
	/// \param name The name to match against. Expected format is name(mod type1, mod type2)
	/// \return True if the name and parameters match.
	bool matchName(std::wstring_view name) const override;

	size_t getParameterCount() const override;
	std::optional <TypeUsage> getParameterType(size_t index) const override;
	std::shared_ptr <Parameter> getParameter(size_t index) const;

	NodeStorage parameters;
};

class AbstractFunction : public AbstractNode
{
public:
	virtual const AbstractTypeUsage& getReturnType() const = 0;

	virtual size_t getParameterCount() const = 0;
	virtual const AbstractParameter& getParameter(size_t index) const = 0;

protected:
	AbstractFunction(std::wstring_view name)
		: AbstractNode(Node::Type::Function, name)
	{
	}
};

}

#endif
