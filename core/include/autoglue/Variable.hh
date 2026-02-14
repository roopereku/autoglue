#ifndef AUTOGLUE_VARIABLE_HH
#define AUTOGLUE_VARIABLE_HH

#include <autoglue/Node.hh>
#include <autoglue/TypeUsage.hh>

#include <optional>

namespace ag
{

class Variable : public Node
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return (
			type == Node::Type::Parameter ||
			type == Node::Type::Field
		);
	}

	std::optional <TypeUsage> getInitializerType() const
	{
		return mInitializerType;
	}

	void initializeUsedType(TypeUsage&& usage)
	{
		if (!mInitializerType)
		{
			mInitializerType.emplace(std::move(usage));
		}
	}

protected:
	Variable(std::string_view name, Type type)
		: Node(name, type)
	{
	}

private:
	std::optional <TypeUsage> mInitializerType;
};

class AbstractVariable : public AbstractNode
{
public:
	/// Gets the initializer type of this parameter.
	///
	/// \return The initializer type of this parameter.
	virtual const AbstractTypeUsage& getInitializerType() const = 0;

protected:
	AbstractVariable(Node::Type type, std::string_view name)
		: AbstractNode(type, name)
	{
	}
};

}

#endif
