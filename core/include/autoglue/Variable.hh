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
	Variable(std::wstring&& name, Type type)
		: Node(std::move(name), type)
	{
	}

private:
	std::optional <TypeUsage> mInitializerType;
};


}

#endif
