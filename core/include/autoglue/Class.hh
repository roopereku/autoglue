#ifndef AUTOGLUE_CLASS_HH
#define AUTOGLUE_CLASS_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/TypeDefinition.hh>
#include <autoglue/TypeUsage.hh>

namespace ag
{

class Class : public Node, public TypeDefinition
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Class;
	}

	Class(std::string_view name) :
		Node(name, Node::Type::Class, members),
		TypeDefinition(TypeDefinition::Type::Class),
		members(NodeStorage::withAllTypesExcept({ Node::Type::Scope, Node::Type::EnumValue, Node::Type::Parameter }))
	{
	}

	size_t getBaseTypeCount() const
	{
		return mBaseTypes.size();
	}

	TypeUsage getBaseType(size_t index) const
	{
		return mBaseTypes[index];
	}

	bool matchTypeName(std::string_view name) const override
	{
		return Node::matchName(name);
	}

	NodeStorage members;

private:
	std::vector <TypeUsage> mBaseTypes;
};

class AbstractClass : public AbstractNode
{
public:
	virtual size_t getBaseTypeCount() const = 0;
	virtual const AbstractTypeUsage& getBaseType(size_t index) const = 0;

protected:
	AbstractClass(std::string_view name)
		: AbstractNode(Node::Type::Class, name)
	{
	}
};

}

#endif
