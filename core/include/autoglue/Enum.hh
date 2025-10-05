#ifndef AUTOGLUE_ENUM_HH
#define AUTOGLUE_ENUM_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/TypeDefinition.hh>

namespace ag
{

class Enum: public Node, public TypeDefinition
{
public:
	constexpr static auto NodeType = Node::Type::Enum;

	Enum(std::wstring&& name) :
		Node(std::move(name), NodeType, values),
		TypeDefinition(TypeDefinition::Type::Enum),
		values(NodeStorage::withTypes({ Node::Type::EnumValue }))
	{
	}

	NodeStorage values;
};

}

#endif
