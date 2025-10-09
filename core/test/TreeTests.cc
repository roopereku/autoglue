#include <autoglue/Tree.hh>
#include <autoglue/Integer.hh>
#include <autoglue/Scope.hh>
#include <autoglue/Class.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Field.hh>
#include <autoglue/Function.hh>

#include <gtest/gtest.h>

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

	AbstractClass& getClass() const override;
	AbstractEnum& getEnum() const override;
	Integer getIntegerDefinition() const override;

private:
	std::weak_ptr <TestNode> mNode;
	TypeDefinition& mDefinition;
};

class TestNode : public std::enable_shared_from_this <TestNode>
{
public:
	TestNode(AbstractNode* abstract)
		: mAbstract(abstract)
	{
	}

	void setInner(TestNodeList&& inner)
	{
		mInner = std::move(inner);
		for (auto& inner : mInner)
		{
			inner->mParent = weak_from_this();
		}
	}

	AbstractNode* mAbstract;
	std::weak_ptr <TestNode> mParent;
	std::shared_ptr <Node> mNode;
	TestNodeList mInner;
};

class TestClass : public AbstractClass, public TestNode
{
public:
	TestClass(std::wstring&& name)
		: AbstractClass(std::move(name)), TestNode(this)
	{
	}

	const AbstractNode& getParent() const override
	{
		return *mParent.lock()->mAbstract;
	}

	using NodeType = Class;
};

class TestScope : public AbstractScope, public TestNode
{
public:
	TestScope(std::wstring&& name)
		: AbstractScope(std::move(name)), TestNode(this)
	{
	}

	const AbstractNode& getParent() const override
	{
		return *mParent.lock()->mAbstract;
	}

	using NodeType = Scope;
};

class TestFunction : public AbstractFunction, public TestNode
{
public:
	TestFunction(std::wstring&& name, TestTypeUsage&& returnType)
		: AbstractFunction(std::move(name)), TestNode(this), mReturnType(std::move(returnType))
	{
	}

	const AbstractNode& getParent() const override
	{
		return *mParent.lock()->mAbstract;
	}

	const AbstractTypeUsage& getReturnType() const override
	{
		return mReturnType;
	}

	size_t getParameterCount() const override
	{
		return 0;
	}

	const AbstractParameter& getParameter(size_t index) const override;

	TestTypeUsage mReturnType;

	using NodeType = Function;
};

class TestParameter : public AbstractParameter, public TestNode
{
public:
	TestParameter(std::wstring&& name, TestTypeUsage&& initializerType)
		: AbstractParameter(std::move(name)), TestNode(this), mInitializerType(std::move(initializerType))
	{
	}

	const AbstractTypeUsage& getInitializerType() const override
	{
		return mInitializerType;
	}

	const AbstractFunction& getParentFunction() const override
	{
		EXPECT_EQ(mParent.lock()->mAbstract->getType(), Node::Type::Function);
		return static_cast <AbstractFunction&> (*mParent.lock()->mAbstract);
	}

	TestTypeUsage mInitializerType;

	using NodeType = Parameter;
};

const AbstractParameter& TestFunction::getParameter(size_t index) const
{
	auto param = std::dynamic_pointer_cast <TestParameter> (mInner[index]);
	EXPECT_TRUE(param);
	return *param;
}

class TestField : public AbstractField, public TestNode
{
public:
	TestField(std::wstring&& name)
		: AbstractField(std::move(name)), TestNode(this)
	{
	}

	std::shared_ptr <TestField> setInitializerType(TestTypeUsage&& usage)
	{
		mInitializerType.emplace(std::move(usage));
		return std::static_pointer_cast <TestField> (shared_from_this());
	}
	
	std::optional <TestTypeUsage> mInitializerType;

	using NodeType = Parameter;
};


template <typename T, typename... Args>
std::shared_ptr <T> makeNode(std::wstring_view name, TestNodeList&& inner = {})
{
	auto testNode = std::make_shared <T> (std::wstring(name));
	testNode->mNode = std::make_shared <typename T::NodeType> (std::wstring(name));
	testNode->setInner(std::move(inner));

	return testNode;
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
			//buildHierarchy(*node);
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
	ASSERT_EQ(node->mAbstract->getType(), matching->getType());
	ASSERT_STREQ(node->mAbstract->getName().c_str(), matching->getName().c_str());

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
		makeNode <TestScope> (L"", {
			makeNode <TestScope> (L"Scope1", {
				makeNode <TestScope> (L"Scope1_1"),
				makeNode <TestScope> (L"Scope1_2")
			}),

			makeNode <TestScope> (L"Scope2", {
				makeNode <TestScope> (L"Scope2_1")
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
		makeNode <TestScope> (L"", {
			makeNode <TestClass> (L"foo", {
				makeNode <TestField> (L"field1")
					->setInitializerType(TestTypeUsage(uint64Definition, {}, false, false)),

				makeNode <TestField> (L"field2")
					->setInitializerType(TestTypeUsage(uint64Definition, {}, false, false)),

				makeNode <TestField> (L"field3")
					->setInitializerType(TestTypeUsage(uint64Definition, {}, false, false)),
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
		makeNode <TestScope> (L"", {
			makeNode <TestFunction> (L"foo", {
				makeNode <TestParameter> (L"param1"),
				makeNode <TestParameter> (L"param2"),
				makeNode <TestParameter> (L"param3")
			}),

			makeNode <TestClass> (L"baz")
		})
	);

	auto bazAbstract = tree.mRoot->mInner[1];
	auto fooAbstract = tree.mRoot->mInner[0];

	auto bazClass = bazAbstract->mNode->as <Class> ();
	ASSERT_TRUE(bazClass);

	// Set function return value and parameters.
	//fooAbstract->setAssociatedType(*bazClass, bazAbstract, false, false);
	//fooAbstract->mInner[0]->setAssociatedType(int16Definition, {}, false, false);
	//fooAbstract->mInner[1]->setAssociatedType(uint64Definition, {}, false, true);
	//fooAbstract->mInner[2]->setAssociatedType(*bazClass, bazAbstract, true, true);

	auto root = tree.build();
	check(tree.mRoot, root);
}
