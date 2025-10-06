#include <autoglue/Tree.hh>
#include <autoglue/Integer.hh>
#include <autoglue/Class.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Field.hh>
#include <autoglue/Function.hh>

#include <gtest/gtest.h>

#include <optional>

using namespace ag;

class TestNode;
using TestNodeList = std::vector <std::shared_ptr <TestNode>>;

class TestTypeUsage : public AbstractTypeUsage
{
public:
	TestTypeUsage(TypeDefinition& definition, std::weak_ptr <TestNode> typeNode, bool isConst, bool isReference)
		: AbstractTypeUsage(definition.getType()), mNode(typeNode), mDefinition(definition)
	{
		mConst = isConst;
		mReference = isReference;
	}

	AbstractNode& getDeclarationOfUsed() const override;
	Integer getIntegerDefinition() const override;

private:
	std::weak_ptr <TestNode> mNode;
	TypeDefinition& mDefinition;
};

class TestNode : public AbstractNode, public std::enable_shared_from_this <TestNode>
{
public:
	TestNode(std::shared_ptr <Node>&& node, TestNodeList&& inner = {})
		: AbstractNode(node->getType(), node->getName()), mNode(std::move(node)), mInner(std::move(inner))
	{
		for (auto& inner : mInner)
		{
			inner->mParent = this;
		}
	}

	std::shared_ptr <TestNode> setAssociatedType(TypeDefinition& definition, std::weak_ptr <TestNode> node, bool isConst, bool isReference)
	{
		associatedType.emplace(definition, node, isConst, isReference);
		return shared_from_this();
	}

	AbstractNode& getParent() const override { return *mParent; }

	const AbstractTypeUsage& getFunctionReturnType() const override
	{
		EXPECT_TRUE(associatedType);
		return *associatedType;
	}

	const AbstractTypeUsage& getVariableInitializerType() const override
	{
		EXPECT_TRUE(associatedType);
		return *associatedType;
	}

	std::shared_ptr <Node> mNode;
	TestNodeList mInner;
	TestNode* mParent = nullptr;

	std::optional <TestTypeUsage> associatedType;
};

AbstractNode& TestTypeUsage::getDeclarationOfUsed() const
{
	EXPECT_FALSE(mNode.expired());
	return *mNode.lock();
}

Integer TestTypeUsage::getIntegerDefinition() const
{
	EXPECT_EQ(mDefinition.getType(), TypeDefinition::Type::Integer);
	return static_cast <Integer&> (mDefinition);
}

template <typename T>
std::shared_ptr <TestNode> makeNode(std::wstring&& name, TestNodeList&& inner = {})
{
	return std::make_shared <TestNode> (std::make_shared <T> (std::move(name)), std::move(inner));
}

class TestTree : public Tree
{
public:
	TestTree(std::shared_ptr <TestNode>&& root)
		: mRoot(root)
	{
	}

	bool onBuild() override
	{
		traverse(mRoot);
		return true;
	}

	void traverse(std::shared_ptr <TestNode> node)
	{
		if (node->mInner.empty())
		{
			buildHierarchy(*node);
		}

		else
		{
			for (auto& inner : node->mInner)
			{
				traverse(inner);
			}
		}
	}

	std::shared_ptr <TestNode> mRoot;
};

void check(std::shared_ptr <TestNode> node, std::shared_ptr <Node> matching)
{
	ASSERT_EQ(node->getType(), matching->getType());
	ASSERT_STREQ(node->getName().c_str(), matching->getName().c_str());

	// TODO: Check node specifics.
	// TODO: Compare TypeUsage of abstract node and real node.

	auto& storage = matching->getStorage();
	std::vector <std::shared_ptr <Node>> inner(storage.begin(), storage.end());

	ASSERT_EQ(node->mInner.size(), inner.size());
	for (size_t i = 0; i < inner.size(); i++)
	{
		check(node->mInner[i], inner[i]);
	}
}

TEST(TreeTests, BuildNestedScopes)
{
	TestTree tree(
		makeNode <Scope> (L"", {
			makeNode <Scope> (L"Scope1", {
				makeNode <Scope> (L"Scope1_1"),
				makeNode <Scope> (L"Scope1_2")
			}),

			makeNode <Scope> (L"Scope2", {
				makeNode <Scope> (L"Scope2_1")
			})
		})
	);

	auto root = tree.build();
	check(tree.mRoot, root);
}

TEST(TreeTests, BuildClassWithFields)
{
	Integer uint64Definition(8, true);

	TestTree tree(
		makeNode <Scope> (L"", {
			makeNode <Class> (L"foo", {
				makeNode <Field> (L"field1")
					->setAssociatedType(uint64Definition, {}, false, false),

				makeNode <Field> (L"field2")
					->setAssociatedType(uint64Definition, {}, false, false),

				makeNode <Field> (L"field3")
					->setAssociatedType(uint64Definition, {}, false, false),
			}),
		})
	);

	auto root = tree.build();
	check(tree.mRoot, root);
}

TEST(TreeTests, BuildFunctionWithDifferentTypes)
{
	Integer uint64Definition(8, true);
	Integer int16Definition(2, false);

	TestTree tree(
		makeNode <Scope> (L"", {
			makeNode <Function> (L"foo", {
				makeNode <Parameter> (L"param1"),
				makeNode <Parameter> (L"param2"),
				makeNode <Parameter> (L"param3")
			}),

			makeNode <Class> (L"baz")
		})
	);

	auto bazAbstract = tree.mRoot->mInner[1];
	auto fooAbstract = tree.mRoot->mInner[0];

	auto bazClass = bazAbstract->mNode->as <Class> ();
	ASSERT_TRUE(bazClass);

	// Set function return value and parameters.
	fooAbstract->setAssociatedType(*bazClass, bazAbstract, false, false);
	fooAbstract->mInner[0]->setAssociatedType(int16Definition, {}, false, false);
	fooAbstract->mInner[1]->setAssociatedType(uint64Definition, {}, false, true);
	fooAbstract->mInner[2]->setAssociatedType(*bazClass, bazAbstract, true, true);

	auto root = tree.build();
	check(tree.mRoot, root);
}
