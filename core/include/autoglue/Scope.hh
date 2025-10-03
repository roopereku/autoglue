#ifndef AUTOGLUE_SCOPE_HH
#define AUTOGLUE_SCOPE_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>

namespace ag
{

class Scope: public Node
{
public:
	constexpr static Type NodeType = Type::Scope;

	Scope(std::wstring&& name) :
		Node(std::move(name), NodeType, children),
		children(NodeStorage::withAllTypesExcept({ Type::EnumValue }))
	{
	}

	NodeStorage children;
};

}

#endif
