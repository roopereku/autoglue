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
	ASSERT_FALSE(usage1.matchName(L"float   \t const"));
	ASSERT_FALSE(usage1.matchName(L"\n  ref float  "));
	ASSERT_FALSE(usage1.matchName(L"  const \t\t\tref float"));
	ASSERT_TRUE(usage1.matchName(L"  const  float"));

	Class classDefinition(L"testClass");
	auto usage2 = TypeUsage(classDefinition, TypeModifierHolder(true, false));
	ASSERT_FALSE(usage2.matchName(L"testClass const"));
	ASSERT_FALSE(usage2.matchName(L"  ref testClass"));
	ASSERT_FALSE(usage2.matchName(L"const ref   testClass\n\n"));
	ASSERT_TRUE(usage2.matchName(L"\n  \t\nconst   testClass"));
}

TEST(TypeUsageTests, matchOnlyReference)
{
	Integer uint32Definition(4, true);
	auto usage1 = TypeUsage(uint32Definition, TypeModifierHolder(false, true));
	ASSERT_FALSE(usage1.matchName(L"  uint32  ref\t\n"));
	ASSERT_FALSE(usage1.matchName(L"const uint32  "));
	ASSERT_FALSE(usage1.matchName(L"const ref uint32"));
	ASSERT_TRUE(usage1.matchName(L"ref   uint32\n\t"));

	Class classDefinition(L"testClass");
	auto usage2 = TypeUsage(classDefinition, TypeModifierHolder(false, true));
	ASSERT_FALSE(usage2.matchName(L"testClass ref   \t\n"));
	ASSERT_FALSE(usage2.matchName(L"  const testClass"));
	ASSERT_FALSE(usage2.matchName(L"const ref testClass"));
	ASSERT_TRUE(usage2.matchName(L"ref   testClass"));
}

TEST(TypeUsageTests, matchConstAndReference)
{
	TypeDefinition voidDefinition(TypeDefinition::Type::Void);
	auto usage1 = TypeUsage(voidDefinition, TypeModifierHolder(true, true));
	ASSERT_FALSE(usage1.matchName(L"void const ref"));
	ASSERT_FALSE(usage1.matchName(L"void ref const"));
	ASSERT_FALSE(usage1.matchName(L"ref const void"));
	ASSERT_FALSE(usage1.matchName(L"ref void const"));
	ASSERT_FALSE(usage1.matchName(L"const void ref"));
	ASSERT_TRUE(usage1.matchName(L"const ref void"));

	Enum enumDefinition(L"testEnum");
	auto usage2 = TypeUsage(enumDefinition, TypeModifierHolder(true, true));
	ASSERT_FALSE(usage2.matchName(L"testEnum const ref"));
	ASSERT_FALSE(usage2.matchName(L"testEnum ref const"));
	ASSERT_FALSE(usage2.matchName(L"ref const testEnum"));
	ASSERT_FALSE(usage2.matchName(L"ref testEnum const"));
	ASSERT_FALSE(usage2.matchName(L"const testEnum ref"));
	ASSERT_TRUE(usage2.matchName(L"const ref testEnum"));
}

TEST(TypeUsageTests, matchIntegerNames)
{
	for (size_t bytes = 1; bytes <= 8; bytes *= 2)
	{
		for (size_t isUnsigned = 0; bytes <= 1; bytes++)
		{
			Integer definition(bytes, isUnsigned);
			TypeUsage usage(definition, TypeModifierHolder(false, false));

			std::wstring name = (isUnsigned ? L"uint" : L"int") + std::to_wstring(bytes * 8);
			ASSERT_TRUE(usage.matchName(name));
		}
	}
}
