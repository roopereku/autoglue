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
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Enum;
	}

	Enum(std::wstring&& name) :
		Node(std::move(name), Node::Type::Enum, values),
		TypeDefinition(TypeDefinition::Type::Enum),
		values(NodeStorage::withTypes({ Node::Type::EnumValue }))
	{
	}

	bool matchTypeName(std::wstring_view name) const override
	{
		return Node::matchName(name);
	}

	NodeStorage values;
};

class AbstractEnum : public AbstractNode
{
public:
	virtual const AbstractTypeUsage& getValueType() const = 0;

protected:
	AbstractEnum(std::wstring&& name)
		: AbstractNode(Node::Type::Enum, std::move(name))
	{
	}
};

}

#endif
