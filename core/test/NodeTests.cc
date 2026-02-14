#include <autoglue/Scope.hh>
#include <autoglue/Class.hh>
#include <autoglue/Function.hh>
#include <autoglue/Enum.hh>
#include <autoglue/EnumValue.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Field.hh>

#include <gtest/gtest.h>

using namespace ag;

TEST(NodeTests, VariableSubTypes)
{
	auto parameterNode = std::make_shared <Parameter> ("");
	ASSERT_TRUE(parameterNode->as <Variable> ());

	auto fieldNode = std::make_shared <Field> ("");
	ASSERT_TRUE(fieldNode->as <Variable> ());
}

TEST(NodeTests, ScopeChildTypes)
{
	auto node = std::make_shared <Scope> ("scope");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->children.ensure(nodeType, "child" + std::to_string(i));

		if (nodeType == Node::Type::EnumValue ||
			nodeType == Node::Type::Parameter ||
			nodeType == Node::Type::Field )
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
	auto node = std::make_shared <Class> ("class");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->members.ensure(nodeType, "child" + std::to_string(i));

		if (nodeType == Node::Type::EnumValue ||
			nodeType == Node::Type::Scope ||
			nodeType == Node::Type::Parameter)

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
	auto node = std::make_shared <Function> ("function");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->parameters.ensure(nodeType, "child" + std::to_string(i));

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
	auto node = std::make_shared <Enum> ("enum");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->values.ensure(nodeType, "child" + std::to_string(i));

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
	auto node = std::make_shared <EnumValue> ("enum_value");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->getStorage().ensure(nodeType, "child" + std::to_string(i));

		ASSERT_FALSE(result);
	}
}

TEST(NodeTests, ParameterChildTypes)
{
	auto node = std::make_shared <Parameter> ("parameter");
	for (size_t i = 0; i < Node::TypeCount; i++)
	{
		const auto nodeType = static_cast <Node::Type> (i);
		const auto result = node->getStorage().ensure(nodeType, "child" + std::to_string(i));

		ASSERT_FALSE(result);
	}
}

TEST(NodeTests, FindNode)
{
	auto root = std::make_shared <Scope> ("");

	root->children.ensure(Node::Type::Scope, "Scope1");
	auto scope1 = root->find("Scope1");
	ASSERT_NE(scope1, nullptr);
	ASSERT_STREQ(scope1->getName().c_str(), "Scope1");

	scope1->as <ag::Scope> ()->children.ensure(Node::Type::Scope, "Scope2");
	auto scope2 = root->find("Scope1.Scope2");
	ASSERT_NE(scope2, nullptr);
	ASSERT_STREQ(scope2->getName().c_str(), "Scope2");

	scope2 = root->find("Scope1\t\n  . \t\nScope2");
	ASSERT_NE(scope2, nullptr);
	ASSERT_STREQ(scope2->getName().c_str(), "Scope2");
}

TEST(NodeTests, FindInvalidNode)
{
	auto root = std::make_shared <ag::Scope> ("");

	auto invalid = root->find("Scope1");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find("");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find("...");
	ASSERT_EQ(invalid, nullptr);

	root->children.ensure(Node::Type::Scope, "Scope1");

	invalid = root->find("Scope1...");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find("Scope1.Scope2");
	ASSERT_EQ(invalid, nullptr);
	invalid = root->find("Scope1.");
	ASSERT_EQ(invalid, nullptr);
}
