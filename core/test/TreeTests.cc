#include <autoglue/Tree.hh>
#include <autoglue/Integer.hh>

#include <gtest/gtest.h>

#include <optional>

using namespace ag;

class TestNode;

class TestTypeUsage : public AbstractTypeUsage
{
public:
	TestTypeUsage(TypeDefinition& definition, TestNode& typeNode, bool isConst, bool isReference)
		: AbstractTypeUsage(definition.getType()), mNode(typeNode), mDefinition(definition)
	{
		mConst = isConst;
		mReference = isReference;
	}

	AbstractNode& getDeclarationOfUsed() const override;
	Integer getIntegerDefinition() const override;

private:
	TestNode& mNode;
	TypeDefinition& mDefinition;
};

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

	TestNode& setAssociatedType(TypeDefinition& definition, TestNode& node, bool isConst, bool isReference)
	{
		associatedType.emplace(definition, node, isConst, isReference);
		return *this;
	}

	const std::wstring& getName() const override { return mNode->getName(); }
	Node::Type getType() const override { return mNode->getType(); }
	AbstractNode& getParent() const override { return *mParent; }
	bool hasParent() const override { return mParent; }

	const AbstractTypeUsage& getFunctionReturnType() const override
	{
		return *associatedType;
	}

	const AbstractTypeUsage& getVariableInitializerType() const override
	{
		return *associatedType;
	}

	std::shared_ptr <Node> mNode;
	std::vector <TestNode> mInner;
	TestNode* mParent = nullptr;

	std::optional <TestTypeUsage> associatedType;
};

AbstractNode& TestTypeUsage::getDeclarationOfUsed() const
{
	return mNode;
}

Integer TestTypeUsage::getIntegerDefinition() const
{
	EXPECT_EQ(mDefinition.getType(), TypeDefinition::Type::Integer);
	return static_cast <Integer&> (mDefinition);
}

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
