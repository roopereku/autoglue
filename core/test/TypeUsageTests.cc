#include <autoglue/TypeUsage.hh>
#include <autoglue/Integer.hh>
#include <autoglue/Class.hh>
#include <autoglue/Enum.hh>

#include <gtest/gtest.h>

using namespace ag;

TEST(TypeUsageTests, matchOnlyConst)
{
	TypeDefinition floatDefinition(TypeDefinition::Type::Float);
	auto usage1 = TypeUsage(floatDefinition, TypeModifierHolder(true, false));
	ASSERT_FALSE(usage1.matchName("float   \t const"));
	ASSERT_FALSE(usage1.matchName("\n  ref float  "));
	ASSERT_FALSE(usage1.matchName("  const \t\t\tref float"));
	ASSERT_TRUE(usage1.matchName("  const  float"));

	Class classDefinition("testClass");
	auto usage2 = TypeUsage(classDefinition, TypeModifierHolder(true, false));
	ASSERT_FALSE(usage2.matchName("testClass const"));
	ASSERT_FALSE(usage2.matchName("  ref testClass"));
	ASSERT_FALSE(usage2.matchName("const ref   testClass\n\n"));
	ASSERT_TRUE(usage2.matchName("\n  \t\nconst   testClass"));
}

TEST(TypeUsageTests, matchOnlyReference)
{
	Integer uint32Definition(4, true);
	auto usage1 = TypeUsage(uint32Definition, TypeModifierHolder(false, true));
	ASSERT_FALSE(usage1.matchName("  uint32  ref\t\n"));
	ASSERT_FALSE(usage1.matchName("const uint32  "));
	ASSERT_FALSE(usage1.matchName("const ref uint32"));
	ASSERT_TRUE(usage1.matchName("ref   uint32\n\t"));

	Class classDefinition("testClass");
	auto usage2 = TypeUsage(classDefinition, TypeModifierHolder(false, true));
	ASSERT_FALSE(usage2.matchName("testClass ref   \t\n"));
	ASSERT_FALSE(usage2.matchName("  const testClass"));
	ASSERT_FALSE(usage2.matchName("const ref testClass"));
	ASSERT_TRUE(usage2.matchName("ref   testClass"));
}

TEST(TypeUsageTests, matchConstAndReference)
{
	TypeDefinition voidDefinition(TypeDefinition::Type::Void);
	auto usage1 = TypeUsage(voidDefinition, TypeModifierHolder(true, true));
	ASSERT_FALSE(usage1.matchName("void const ref"));
	ASSERT_FALSE(usage1.matchName("void ref const"));
	ASSERT_FALSE(usage1.matchName("ref const void"));
	ASSERT_FALSE(usage1.matchName("ref void const"));
	ASSERT_FALSE(usage1.matchName("const void ref"));
	ASSERT_TRUE(usage1.matchName("const ref void"));

	Enum enumDefinition("testEnum");
	auto usage2 = TypeUsage(enumDefinition, TypeModifierHolder(true, true));
	ASSERT_FALSE(usage2.matchName("testEnum const ref"));
	ASSERT_FALSE(usage2.matchName("testEnum ref const"));
	ASSERT_FALSE(usage2.matchName("ref const testEnum"));
	ASSERT_FALSE(usage2.matchName("ref testEnum const"));
	ASSERT_FALSE(usage2.matchName("const testEnum ref"));
	ASSERT_TRUE(usage2.matchName("const ref testEnum"));
}

TEST(TypeUsageTests, matchIntegerNames)
{
	for (size_t bytes = 1; bytes <= 8; bytes *= 2)
	{
		for (size_t isUnsigned = 0; bytes <= 1; bytes++)
		{
			Integer definition(bytes, isUnsigned);
			TypeUsage usage(definition, TypeModifierHolder(false, false));

			std::string name = (isUnsigned ? "uint" : "int") + std::to_string(bytes * 8);
			ASSERT_TRUE(usage.matchName(name));
		}
	}
}
