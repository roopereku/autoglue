#include <autoglue/Tree.hh>
#include <autoglue/Integer.hh>
#include <autoglue/Scope.hh>
#include <autoglue/Class.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Field.hh>
#include <autoglue/Function.hh>
#include <autoglue/Enum.hh>
#include <autoglue/Integer.hh>

#include <gtest/gtest.h>

using namespace ag;

class TestNode;
using TestNodeList = std::vector <std::shared_ptr <TestNode>>;

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

	TypeDefinition::Type whichTypeDefinition() const
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

	virtual void testSpecific(std::shared_ptr <Node> matching) const = 0;

	AbstractNode* mAbstract;
	std::weak_ptr <TestNode> mParent;
	TestNodeList mInner;
};

class TestTypeUsage : public AbstractTypeUsage
{
public:
	TestTypeUsage(std::weak_ptr <TestNode> decl, bool isConst, bool isReference)
		: AbstractTypeUsage(decl.lock()->whichTypeDefinition()), mNode(decl)
	{
		mConst = isConst;
		mReference = isReference;
	}

	TestTypeUsage(TypeDefinition& definition, bool isConst, bool isReference)
		: AbstractTypeUsage(definition.getType()), mDefinition(&definition)
	{
		mConst = isConst;
		mReference = isReference;
	}

	void test(const TypeUsage& real) const
	{
		ASSERT_EQ(mConst, real.isConst());
		ASSERT_EQ(mReference, real.isReference());
		ASSERT_EQ(getTypeOfUsedDefinition(), real.getUsedType().getType());
	}

	const AbstractClass& getClass() const override;
	const AbstractEnum& getEnum() const override;
	const Integer getIntegerDefinition() const override;

private:
	std::weak_ptr <TestNode> mNode;
	TypeDefinition* mDefinition = nullptr;
};

class TestClass : public AbstractClass, public TestNode
{
public:
	TestClass(std::wstring&& name)
		: AbstractClass(std::move(name)), TestNode(this)
	{
	}

	size_t getBaseTypeCount() const override
	{
		return mBaseTypes.size();
	}
	
	const AbstractTypeUsage& getBaseType(size_t index) const override
	{
		return mBaseTypes[index];
	}

	const AbstractNode& getParent() const override
	{
		return *mParent.lock()->mAbstract;
	}

	void testSpecific(std::shared_ptr <Node> matching) const override
	{
		auto matchingClass = matching->as <Class> ();
		ASSERT_TRUE(matchingClass);

		ASSERT_EQ(mBaseTypes.size(), matchingClass->getBaseTypeCount());
		for (size_t i = 0; i < mBaseTypes.size(); i++)
		{
			mBaseTypes[i].test(matchingClass->getBaseType(i));
		}
	}

	std::vector <TestTypeUsage> mBaseTypes;
};

class TestEnum : public AbstractEnum, public TestNode
{
public:
	TestEnum(std::wstring&& name)
		: AbstractEnum(std::move(name)), TestNode(this)
	{
	}

	const AbstractNode& getParent() const override
	{
		return *mParent.lock()->mAbstract;
	}

	void testSpecific(std::shared_ptr <Node> matching) const override
	{
		auto matchingEnum = matching->as <Enum> ();
		ASSERT_TRUE(matchingEnum);

		// TODO: Test value type.
	}
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

	void testSpecific(std::shared_ptr <Node> matching) const override
	{
		auto matchingScope = matching->as <Scope> ();
		ASSERT_TRUE(matchingScope);
	}
};

class TestFunction : public AbstractFunction, public TestNode
{
public:
	TestFunction(std::wstring&& name)
		: AbstractFunction(std::move(name)), TestNode(this)
	{
	}

	const AbstractNode& getParent() const override
	{
		return *mParent.lock()->mAbstract;
	}

	void testSpecific(std::shared_ptr <Node> matching) const override
	{
		auto matchingFunction = matching->as <Function> ();
		ASSERT_TRUE(matchingFunction);

		ASSERT_TRUE(mReturnType);
		ASSERT_TRUE(matchingFunction->getReturnType());
		mReturnType->test(*matchingFunction->getReturnType());
	}

	const AbstractTypeUsage& getReturnType() const override
	{
		return *mReturnType;
	}

	std::shared_ptr <TestFunction> setReturnType(TestTypeUsage&& usage)
	{
		mReturnType.emplace(std::move(usage));
		return std::static_pointer_cast <TestFunction> (shared_from_this());
	}

	size_t getParameterCount() const override
	{
		return mInner.size();
	}

	const AbstractParameter& getParameter(size_t index) const override;

	std::optional <TestTypeUsage> mReturnType;
};

class TestParameter : public AbstractParameter, public TestNode
{
public:
	TestParameter(std::wstring&& name)
		: AbstractParameter(std::move(name)), TestNode(this)
	{
	}

	const AbstractTypeUsage& getInitializerType() const override
	{
		return *mInitializerType;
	}

	const AbstractFunction& getParentFunction() const override
	{
		EXPECT_EQ(mParent.lock()->mAbstract->getType(), Node::Type::Function);
		return static_cast <AbstractFunction&> (*mParent.lock()->mAbstract);
	}

	void testSpecific(std::shared_ptr <Node> matching) const override
	{
		auto matchingParameter = matching->as <Parameter> ();
		ASSERT_TRUE(matchingParameter);

		ASSERT_TRUE(mInitializerType);
		ASSERT_TRUE(matchingParameter->getInitializerType());
		mInitializerType->test(*matchingParameter->getInitializerType());
	}

	std::shared_ptr <TestParameter> setInitializerType(TestTypeUsage&& usage)
	{
		mInitializerType.emplace(std::move(usage));
		return std::static_pointer_cast <TestParameter> (shared_from_this());
	}

	std::optional <TestTypeUsage> mInitializerType;
};

const AbstractParameter& TestFunction::getParameter(size_t index) const
{
	auto param = mInner[index]->mAbstract;
	EXPECT_TRUE(param->getType() == Node::Type::Parameter);
	return static_cast <AbstractParameter&> (*param);
}

const AbstractClass& TestTypeUsage::getClass() const
{
	EXPECT_FALSE(mNode.expired());
	return *std::static_pointer_cast <TestClass> (mNode.lock());
}

const AbstractEnum& TestTypeUsage::getEnum() const
{
	EXPECT_FALSE(mNode.expired());
	return *std::static_pointer_cast <TestEnum> (mNode.lock());
}

const Integer TestTypeUsage::getIntegerDefinition() const
{
	return static_cast <Integer&> (*mDefinition);
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

	const AbstractTypeUsage& getInitializerType() const override
	{
		return *mInitializerType;
	}

	const AbstractClass& getParentClass() const override
	{
		EXPECT_EQ(mParent.lock()->mAbstract->getType(), Node::Type::Class);
		return static_cast <AbstractClass&> (*mParent.lock()->mAbstract);
	}

	void testSpecific(std::shared_ptr <Node> matching) const override
	{
		auto matchingField = matching->as <Field> ();
		ASSERT_TRUE(matchingField);

		ASSERT_TRUE(mInitializerType);
		ASSERT_TRUE(matchingField->getInitializerType());
		mInitializerType->test(*matchingField->getInitializerType());
	}
	
	std::optional <TestTypeUsage> mInitializerType;
};


template <typename T, typename... Args>
std::shared_ptr <T> makeNode(std::wstring_view name, TestNodeList&& inner = {})
{
	auto testNode = std::make_shared <T> (std::wstring(name));
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
		switch(node->mAbstract->getType())
		{
			case Node::Type::Function:
			{
				build(*std::static_pointer_cast <TestFunction> (node));
				break;
			}

			case Node::Type::Enum:
			{
				build(*std::static_pointer_cast <TestEnum> (node));
				break;
			}

			case Node::Type::Class:
			{
				build(*std::static_pointer_cast <TestClass> (node));
				break;
			}

			case Node::Type::Field:
			{
				build(*std::static_pointer_cast <TestField> (node));
				break;
			}

			default:
			{
			}
		}

		for (auto& inner : node->mInner)
		{
			traverse(inner);
		}
	}

	std::shared_ptr <TestNode> mRoot;
};

void testNode(std::shared_ptr <TestNode> node, std::shared_ptr <Node> matching)
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

TEST(TreeTests, ExclusivelyScopesBuildsNothing)
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
	ASSERT_EQ(root->children.begin(), root->children.end());
}

TEST(TreeTests, BuildClassWithFields)
{
	Integer uint64Definition(8, true);

	TestTree tree(
		makeNode <TestScope> (L"", {
			makeNode <TestClass> (L"foo", {
				makeNode <TestField> (L"field1")
					->setInitializerType(TestTypeUsage(uint64Definition, false, false)),

				makeNode <TestField> (L"field2")
					->setInitializerType(TestTypeUsage(uint64Definition, false, false)),

				makeNode <TestField> (L"field3")
					->setInitializerType(TestTypeUsage(uint64Definition, false, false)),
			}),
		})
	);

	auto root = tree.build();
	testNode(tree.mRoot, root);
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

	ASSERT_EQ(tree.mRoot->mInner[0]->mAbstract->getType(), Node::Type::Function);
	auto fooAbstract = std::static_pointer_cast <TestFunction> (tree.mRoot->mInner[0]);

	ASSERT_EQ(tree.mRoot->mInner[1]->mAbstract->getType(), Node::Type::Class);
	auto bazAbstract = std::static_pointer_cast <TestClass> (tree.mRoot->mInner[1]);

	// Set function return value and parameters.
	fooAbstract->setReturnType(TestTypeUsage(bazAbstract, false, false));

	std::array <TestTypeUsage, 3> paramTypes
	{
		TestTypeUsage(int16Definition, false, false),
		TestTypeUsage(uint64Definition, false, true),
		TestTypeUsage(bazAbstract, true, true)
	};

	for (size_t i = 0; i < paramTypes.size(); i++)
	{
		ASSERT_EQ(fooAbstract->mInner[i]->mAbstract->getType(), Node::Type::Parameter);
		std::static_pointer_cast <TestParameter> (fooAbstract->mInner[i])->setInitializerType(std::move(paramTypes[i]));
	}

	auto root = tree.build();
	testNode(tree.mRoot, root);
}
