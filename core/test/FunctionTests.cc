#include <autoglue/Function.hh>

#include <gtest/gtest.h>

using namespace ag;

TEST(FunctionTests, FunctionLookup)
{
	auto storage = NodeStorage::withTypes({ Node::Type::Function });
	auto node = storage.ensure(Node::Type::Function, L"foo");
	ASSERT_TRUE(node);

	// When incomplete, functions should only be resolvable by name.
	//ASSERT_TRUE(node->isDependency());
	//ASSERT_EQ(storage.getNodeByName(L"foo", Node::Type::Function), node);
	//ASSERT_FALSE(storage.getNodeByName(L"foo()", Node::Type::Function));

	//node->setCompleted();
	//ASSERT_FALSE(node->isDependency());
	//ASSERT_FALSE(storage.getNodeByName(L"foo", Node::Type::Function));
	//ASSERT_EQ(storage.getNodeByName(L"foo()", Node::Type::Function), node);
}
