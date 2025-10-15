#include <autoglue/Scope.hh>
#include <autoglue/Class.hh>
#include <autoglue/Function.hh>
#include <autoglue/Enum.hh>
#include <autoglue/EnumValue.hh>
#include <autoglue/Parameter.hh>

#include <gtest/gtest.h>

using namespace ag;

TEST(NodeTests, CanTypeBeStored)
{
	auto all = NodeStorage::withAllTypes();

	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		auto current = static_cast <Node::Type> (i);
		ASSERT_TRUE(all.canStore(current));

		auto onlyCurrent = NodeStorage::withTypes({current});
		ASSERT_TRUE(onlyCurrent.canStore(current));

		auto notCurrent = NodeStorage::withAllTypesExcept({current});
		ASSERT_FALSE(notCurrent.canStore(current));

		for (size_t j = 0; j < Node::TypeCount; j++)
		{
			if (j != i)
			{
				auto other = static_cast <Node::Type> (j);
				ASSERT_FALSE(onlyCurrent.canStore(other));
				ASSERT_TRUE(notCurrent.canStore(other));
			}
		}
	}
}

TEST(NodeTests, SpecificStorageAvailableFromAbstract)
{
	auto check = [](std::shared_ptr <Node> node, NodeStorage& same)
	{
		ASSERT_EQ(&node->getStorage(), &same);
		ASSERT_NE(&node->getStorage(), &NodeStorage::getDefault());
	};

	auto classNode = std::make_shared <Class> ("name");
	check(classNode, classNode->members);

	auto scopeNode = std::make_shared <Scope> ("name");
	check(scopeNode, scopeNode->children);

	auto enumNode = std::make_shared <Enum> ("name");
	check(enumNode, enumNode->values);

	auto functionNode = std::make_shared <Function> ("name");
	check(functionNode, functionNode->parameters);
}


