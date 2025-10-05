#ifndef AUTOGLUE_PARAMETER_HH
#define AUTOGLUE_PARAMETER_HH

#include <autoglue/Node.hh>
#include <autoglue/TypeUsage.hh>

#include <optional>

namespace ag
{

class Parameter : public Node
{
public:
	static constexpr auto NodeType = Node::Type::Parameter;

	Parameter(std::wstring&& name)
		: Node(std::move(name), Type::Parameter)
	{
	}

	std::optional <TypeUsage> getInitializerType() const
	{
		return mType;
	}

	void setInitializerType(TypeUsage&& usage)
	{
		if (!mType)
		{
			mType.emplace(std::move(usage));
		}
	}

private:
	std::optional <TypeUsage> mType;
};


}

#endif
