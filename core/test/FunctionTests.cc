#include <autoglue/test/Tree.hh>
#include <autoglue/test/FunctionImpl.hh>
#include <autoglue/test/ParameterImpl.hh>
#include <autoglue/test/ScopeImpl.hh>
#include <autoglue/test/ClassImpl.hh>

#include <gtest/gtest.h>

using namespace ag;
using namespace ag::test;

TEST(FunctionTests, FunctionLookup)
{
	const auto validateParameter = [](auto& func, std::shared_ptr <Node> param, auto&& paramType, size_t index)
	{
		ASSERT_EQ(func->getParameterCount(), index + 1);

		ASSERT_TRUE(param);
		auto paramNode = param->as <Parameter> ();
		ASSERT_TRUE(paramNode);
		ASSERT_EQ(func->getParameter(index), paramNode);

		// Set the added parameter to be an int and make sure that a matching function is found.
		paramNode->initializeUsedType(std::move(paramType));
	};

	auto storage = NodeStorage::withTypes({ Node::Type::Function });
	auto node = storage.ensure(Node::Type::Function, "foo");
	ASSERT_TRUE(node);

	auto func = node->as <Function> ();
	ASSERT_TRUE(func);

	// Function without parameters must be found.
	auto found = storage.getNodeByName("foo()");
	ASSERT_TRUE(found);
	ASSERT_EQ(found, func);

	// Function with parameters must not be found.
	ASSERT_FALSE(storage.getNodeByName("foo(int)"));

	Integer int16Definition(2, false);
	Integer uint16Definition(2, true);

	// Make sure that a parameter can be added.
	auto param = func->parameters.ensure(Node::Type::Parameter, "param1");
	validateParameter(func, param, TypeUsage(int16Definition, TypeModifierHolder(false, false)), 0);

	// Function with one int parameter must be found.
	found = storage.getNodeByName("foo(int16)");
	ASSERT_TRUE(found);
	ASSERT_EQ(found, func);

	// Function with no parameters must be not found.
	ASSERT_FALSE(storage.getNodeByName("foo()"));

	// Make sure that more parameters can be added.
	param = func->parameters.ensure(Node::Type::Parameter, "param2");
	validateParameter(func, param, TypeUsage(uint16Definition, TypeModifierHolder(false, false)), 1);

	// Function with one int and uint parameter must be found.
	found = storage.getNodeByName("foo(   int16,  uint16   )");
	ASSERT_TRUE(found);
	ASSERT_EQ(found, func);
}

TEST(TreeTests, BuildFunctionWithDifferentTypes)
{
	Integer uint64Definition(8, true);
	Integer int16Definition(2, false);

	test::Tree tree(
		makeNode <ScopeImpl> ("", {
			makeNode <FunctionImpl> ("foo", {
				makeNode <ParameterImpl> ("param1"),
				makeNode <ParameterImpl> ("param2"),
				makeNode <ParameterImpl> ("param3")
			}),

			makeNode <ClassImpl> ("baz")
		})
	);

	ASSERT_EQ(tree.mRoot->mInner[0]->mAbstract->getType(), Node::Type::Function);
	auto fooAbstract = std::static_pointer_cast <FunctionImpl> (tree.mRoot->mInner[0]);

	ASSERT_EQ(tree.mRoot->mInner[1]->mAbstract->getType(), Node::Type::Class);
	auto bazAbstract = std::static_pointer_cast <ClassImpl> (tree.mRoot->mInner[1]);

	// Set function return value and parameters.
	fooAbstract->setReturnType(TypeUsageImpl(bazAbstract, false, false));

	std::array <TypeUsageImpl, 3> paramTypes
	{
		TypeUsageImpl(int16Definition, false, false),
		TypeUsageImpl(uint64Definition, false, true),
		TypeUsageImpl(bazAbstract, true, true)
	};

	for (size_t i = 0; i < paramTypes.size(); i++)
	{
		ASSERT_EQ(fooAbstract->mInner[i]->mAbstract->getType(), Node::Type::Parameter);
		std::static_pointer_cast <ParameterImpl> (fooAbstract->mInner[i])->setInitializerType(std::move(paramTypes[i]));
	}

	auto root = tree.build();
	testNode(tree.mRoot, root);
}
