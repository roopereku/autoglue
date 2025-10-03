#ifndef AUTOGLUE_CLASS_HH
#define AUTOGLUE_CLASS_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>

namespace ag
{

class Class : public Node
{
public:
	constexpr static Type NodeType = Type::Class;

	Class(std::wstring&& name) :
		Node(std::move(name), NodeType, members),
		members(NodeStorage::withAllTypesExcept({ Type::Scope, Type::EnumValue }))
	{
	}

	NodeStorage members;
};

}

#endif
