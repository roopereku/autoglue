#ifndef AUTOGLUE_TEST_NODE_BASE_HH
#define AUTOGLUE_TEST_NODE_BASE_HH

#include <autoglue/Node.hh>
#include <autoglue/TypeDefinition.hh>

#include <vector>

namespace ag::test
{

class NodeBase;
using NodeList = std::vector <std::shared_ptr <NodeBase>>;

class NodeBase : public std::enable_shared_from_this <NodeBase>
{
public:
	NodeBase(AbstractNode* abstract);

	void setInner(NodeList&& inner);
	std::shared_ptr <NodeBase> addInner(std::shared_ptr <NodeBase> node);
	TypeDefinition::Type whichTypeDefinition() const;

	virtual void testSpecific(std::shared_ptr <Node> matching) const = 0;

	AbstractNode* mAbstract;
	std::weak_ptr <NodeBase> mParent;
	NodeList mInner;
};

template <typename T, typename... Args>
std::shared_ptr <T> makeNode(std::string_view name, NodeList&& inner = {})
{
	auto testNode = std::make_shared <T> (std::string(name));
	testNode->setInner(std::move(inner));

	return testNode;
}

void testNode(std::shared_ptr <NodeBase> node, std::shared_ptr <Node> matching);

}

#endif
