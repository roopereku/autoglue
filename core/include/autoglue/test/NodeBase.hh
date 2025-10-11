#ifndef AUTOGLUE_TEST_NODE_BASE_HH
#define AUTOGLUE_TEST_NODE_BASE_HH

#include <autoglue/AbstractNode.hh>

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
	TypeDefinition::Type whichTypeDefinition() const;

	virtual void testSpecific(std::shared_ptr <Node> matching) const = 0;

	AbstractNode* mAbstract;
	std::weak_ptr <NodeBase> mParent;
	NodeList mInner;
};

template <typename T, typename... Args>
std::shared_ptr <T> makeNode(std::wstring_view name, NodeList&& inner = {})
{
	auto testNode = std::make_shared <T> (std::wstring(name));
	testNode->setInner(std::move(inner));

	return testNode;
}

void testNode(std::shared_ptr <NodeBase> node, std::shared_ptr <Node> matching);

}

#endif
