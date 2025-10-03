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

	auto classNode = std::make_shared <Class> (L"name");
	check(classNode, classNode->members);

	auto scopeNode = std::make_shared <Scope> (L"name");
	check(scopeNode, scopeNode->children);

	auto enumNode = std::make_shared <Enum> (L"name");
	check(enumNode, enumNode->values);

	auto functionNode = std::make_shared <Function> (L"name");
	check(functionNode, functionNode->parameters);
}

TEST(NodeTests, ScopeChildTypes)
{
	auto node = std::make_shared <Scope> (L"scope");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->children.ensure(nodeType, L"child" + std::to_wstring(i));

		if (nodeType == Node::Type::EnumValue)
		{
			ASSERT_FALSE(result);
		}

		else
		{
			ASSERT_TRUE(result);
		}
	}
}

TEST(NodeTests, ClassChildTypes)
{
	auto node = std::make_shared <Class> (L"class");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->members.ensure(nodeType, L"child" + std::to_wstring(i));

		if (nodeType == Node::Type::EnumValue ||
			nodeType == Node::Type::Scope)
		{
			ASSERT_FALSE(result);
		}

		else
		{
			ASSERT_TRUE(result);
		}
	}
}

TEST(NodeTests, FunctionChildTypes)
{
	auto node = std::make_shared <Function> (L"function");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->parameters.ensure(nodeType, L"child" + std::to_wstring(i));

		if (nodeType == Node::Type::Parameter)
		{
			ASSERT_TRUE(result);
		}

		else
		{
			ASSERT_FALSE(result);
		}
	}
}

TEST(NodeTests, EnumChildTypes)
{
	auto node = std::make_shared <Enum> (L"enum");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->values.ensure(nodeType, L"child" + std::to_wstring(i));

		if (nodeType == Node::Type::EnumValue)
		{
			ASSERT_TRUE(result);
		}

		else
		{
			ASSERT_FALSE(result);
		}
	}
}

TEST(NodeTests, EnumValueChildTypes)
{
	auto node = std::make_shared <EnumValue> (L"enum_value");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->getStorage().ensure(nodeType, L"child" + std::to_wstring(i));

		ASSERT_FALSE(result);
	}
}

TEST(NodeTests, ParameterChildTypes)
{
	auto node = std::make_shared <Parameter> (L"parameter");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->getStorage().ensure(nodeType, L"child" + std::to_wstring(i));

		ASSERT_FALSE(result);
	}
}

TEST(NodeTests, FindNode)
{
	auto root = std::make_shared <Scope> (L"");

	root->children.ensure(Node::Type::Scope, L"Scope1");
	auto scope1 = root->find(L"Scope1");
	ASSERT_NE(scope1, nullptr);
	ASSERT_STREQ(scope1->getName().c_str(), L"Scope1");

	scope1->as <ag::Scope> ()->children.ensure(Node::Type::Scope, L"Scope2");
	auto scope2 = root->find(L"Scope1.Scope2");
	ASSERT_NE(scope2, nullptr);
	ASSERT_STREQ(scope2->getName().c_str(), L"Scope2");
}

TEST(NodeTests, FindInvalidNode)
{
	auto root = std::make_shared <ag::Scope> (L"");

	auto invalid = root->find(L"Scope1");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find(L"");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find(L"...");
	ASSERT_EQ(invalid, nullptr);

	root->children.ensure(Node::Type::Scope, L"Scope1");

	invalid = root->find(L"Scope1...");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find(L"Scope1.Scope2");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find(L"Scope1.");
	ASSERT_EQ(invalid, nullptr);
}
