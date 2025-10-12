#ifndef AUTOGLUE_SCOPE_HH
#define AUTOGLUE_SCOPE_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>

namespace ag
{

class Scope: public Node
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Scope;
	}

	Scope(std::wstring_view name) :
		Node(name, Node::Type::Scope, children),
		children(NodeStorage::withAllTypesExcept({ Type::EnumValue, Type::Parameter, Type::Field }))
	{
	}

	NodeStorage children;
};

class AbstractScope : public AbstractNode
{
public:

protected:
	AbstractScope(std::wstring_view name)
		: AbstractNode(Node::Type::Scope, name)
	{
	}
};

}

#endif
