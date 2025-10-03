#include <autoglue/Tree.hh>

#include <gtest/gtest.h>

using namespace ag;

class TestNode : public AbstractNode
{
public:
	TestNode(std::shared_ptr <Node>&& node, std::initializer_list <TestNode>&& inner = {})
		: mNode(std::move(node)), mInner(std::move(inner))
	{
		for (auto& inner : mInner)
		{
			inner.mParent = this;
		}
	}

	const std::wstring& getName() const override { return mNode->getName(); }
	Node::Type getType() const override { return mNode->getType(); }
	AbstractNode& getParent() const override { return *mParent; }
	bool hasParent() const override { return mParent; }

	std::shared_ptr <Node> mNode;
	std::vector <TestNode> mInner;
	TestNode* mParent = nullptr;
};

class TestTree : public Tree
{
public:
	TestTree(const TestNode& root)
		: mRoot(root)
	{
	}

	bool onBuild() override
	{
		traverse(mRoot);
		return true;
	}

	void traverse(TestNode& node)
	{
		if (node.mInner.empty())
		{
			buildHierarchy(node);
		}

		else
		{
			for (auto& inner : node.mInner)
			{
				traverse(inner);
			}
		}
	}

	void check(TestNode& node, std::shared_ptr <TestNode> matching)
	{
		for (auto& inner : node.mInner)
		{
		}
	}
	
	TestNode mRoot;
};

void check(TestNode& node, std::shared_ptr <Node> matching)
{
	ASSERT_EQ(node.getType(), matching->getType());
	ASSERT_STREQ(node.getName().c_str(), matching->getName().c_str());

	// TODO: Check node specifics.

	auto& storage = matching->getStorage();
	std::vector <std::shared_ptr <Node>> inner(storage.begin(), storage.end());

	ASSERT_EQ(node.mInner.size(), inner.size());
	for (size_t i = 0; i < inner.size(); i++)
	{
		check(node.mInner[i], inner[i]);
	}
}

TEST(TreeTests, BuildNestedScopes)
{
	TestTree tree(
		TestNode(std::make_shared <Scope> (L""), {
			TestNode(std::make_shared <Scope> (L"Scope1"), {
				TestNode(std::make_shared <Scope> (L"Scope1_1")),
				TestNode(std::make_shared <Scope> (L"Scope1_2"))
			}),

			TestNode(std::make_shared <Scope> (L"Scope2"), {
				TestNode(std::make_shared <Scope> (L"Scope2_1"))
			}),
		})
	);

	auto root = tree.build();
	check(tree.mRoot, root);
}
