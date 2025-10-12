#include <autoglue/test/NodeBase.hh>
#include <autoglue/NodeStorage.hh>

#include <gtest/gtest.h>

namespace ag::test
{

NodeBase::NodeBase(AbstractNode* abstract)
	: mAbstract(abstract)
{
}

void NodeBase::setInner(NodeList&& inner)
{
	mInner = std::move(inner);
	for (auto& inner : mInner)
	{
		inner->mParent = weak_from_this();
	}
}

std::shared_ptr <NodeBase> NodeBase::addInner(std::shared_ptr <NodeBase> node)
{
	mInner.emplace_back(std::move(node));
	mInner.back()->mParent = weak_from_this();
}

TypeDefinition::Type NodeBase::whichTypeDefinition() const
{
	TypeDefinition::Type result = TypeDefinition::Type::Void;;

	if (mAbstract->getType() == Node::Type::Class)
	{
		result = TypeDefinition::Type::Class;
	}

	else if (mAbstract->getType() == Node::Type::Enum)
	{
		result = TypeDefinition::Type::Enum;
	}

	EXPECT_NE(result, TypeDefinition::Type::Void);
	return result;
}

void testNode(std::shared_ptr <NodeBase> node, std::shared_ptr <Node> matching)
{
	ASSERT_EQ(node->mAbstract->getType(), matching->getType());
	ASSERT_STREQ(node->mAbstract->getName().c_str(), matching->getName().c_str());

	node->testSpecific(matching);

	auto& storage = matching->getStorage();
	std::vector <std::shared_ptr <Node>> inner(storage.begin(), storage.end());

	ASSERT_EQ(node->mInner.size(), inner.size());
	for (size_t i = 0; i < inner.size(); i++)
	{
		testNode(node->mInner[i], inner[i]);
	}
}

}
