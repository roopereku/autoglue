#ifndef AUTOGLUE_CLASS_HH
#define AUTOGLUE_CLASS_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/TypeDefinition.hh>

namespace ag
{

class Class : public Node, public TypeDefinition
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Class;
	}

	Class(std::wstring&& name) :
		Node(std::move(name), Node::Type::Class, members),
		TypeDefinition(TypeDefinition::Type::Class),
		members(NodeStorage::withAllTypesExcept({ Node::Type::Scope, Node::Type::EnumValue, Node::Type::Parameter }))
	{
	}

	NodeStorage members;
};

class AbstractClass : public AbstractNode
{
public:
	virtual size_t getBaseTypeCount() const = 0;
	virtual const AbstractTypeUsage& getBaseType(size_t index) const = 0;

protected:
	AbstractClass(std::wstring&& name)
		: AbstractNode(Node::Type::Class, std::move(name))
	{
	}
};

}

#endif
