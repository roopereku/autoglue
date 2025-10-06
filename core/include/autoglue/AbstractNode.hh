#ifndef AUTOGLUE_ABSTRACT_NODE_HH
#define AUTOGLUE_ABSTRACT_NODE_HH

#include <autoglue/Node.hh>
#include <autoglue/TypeUsage.hh>
#include <autoglue/AbstractTypeUsage.hh>

namespace ag
{

/// AbstractNode is used by source language specific tree building
/// code to provide a temporary source of information about a language specific node.
class AbstractNode
{
public:
	AbstractNode(Node::Type type, std::wstring name)
		: mType(type), mName(name)
	{
	}

	const std::wstring& getName() const
	{
		return mName;
	}

	Node::Type getType() const
	{
		return mType;
	}

	virtual const AbstractNode& getParent() const = 0;

	virtual const AbstractTypeUsage& getFunctionReturnType() const = 0;

	/// Gets the type used for a variable if this node abstraction represents such.
	/// Variables include parameters and class members.
	///
	/// \return Abstract type usage representing the type used for a variable.
	virtual const AbstractTypeUsage& getVariableInitializerType() const = 0;

	/// Determines whether this abstract node represents the global scope.
	/// This must return true for the final parent of all abstract nodes.
	///
	/// \return True if this abstract node represents the global scope.ö
	bool isGlobalScope() const
	{
		return mType == Node::Type::Scope && mName.empty();
	}

private:
	Node::Type mType;
	std::wstring mName;
};

}

#endif
