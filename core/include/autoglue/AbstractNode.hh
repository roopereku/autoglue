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
	const std::wstring& getName() const
	{
		return mName;
	}

	Node::Type getType() const
	{
		return mType;
	}

	// TODO: Refactor this to more speficic versions where the parent has a known type?
	virtual const AbstractNode& getParent() const = 0;

	/// Determines whether this abstract node represents the global scope.
	/// This must return true for the final parent of all abstract nodes.
	///
	/// \return True if this abstract node represents the global scope.ö
	bool isGlobalScope() const
	{
		return mType == Node::Type::Scope && mName.empty();
	}

protected:
	AbstractNode(Node::Type type, std::wstring name)
		: mType(type), mName(name)
	{
	}

private:
	Node::Type mType;
	std::wstring mName;
};

}

#endif
