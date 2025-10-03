#ifndef AUTOGLUE_ABSTRACT_NODE_HH
#define AUTOGLUE_ABSTRACT_NODE_HH

#include <autoglue/Node.hh>

#include <optional>

namespace ag
{

/// AbstractNode is used by source language specific tree building
/// code to provide a temporary source of information about a language specific node.
class AbstractNode
{
public:
	virtual const std::wstring& getName() const = 0;
	virtual Node::Type getType() const = 0;

	virtual AbstractNode& getParent() const = 0;
	virtual bool hasParent() const = 0;
};

}


#endif
