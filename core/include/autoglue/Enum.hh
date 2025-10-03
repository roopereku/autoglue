#ifndef AUTOGLUE_ENUM_HH
#define AUTOGLUE_ENUM_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>

namespace ag
{

class Enum: public Node
{
public:
	constexpr static Type NodeType = Type::Enum;

	Enum(std::wstring&& name) :
		Node(std::move(name), NodeType, values),
		values(NodeStorage::withTypes({ Type::EnumValue }))
	{
	}

	NodeStorage values;
};

}

#endif
