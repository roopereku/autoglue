#include <autoglue/Token.hh>

#include <gtest/gtest.h>

#include <string>

TEST(TokenTests, TrimLeft)
{
	ASSERT_STREQ("no left trim", std::string(ag::trimLeft("no left trim")).c_str());
	ASSERT_STREQ("left trim", std::string(ag::trimLeft("  \t\n\t\n  left trim")).c_str());
}

TEST(TokenTests, TrimRight)
{
	ASSERT_STREQ("no right trim", std::string(ag::trimRight("no right trim")).c_str());
	ASSERT_STREQ("right trim", std::string(ag::trimRight("right trim\t \n  \n\t  ")).c_str());
}

TEST(TokenTests, TrimBoth)
{
	ASSERT_STREQ("left but no right", std::string(ag::trim("      left but no right")).c_str());
	ASSERT_STREQ("right but no left", std::string(ag::trim("right but no left        \n\t")).c_str());
	ASSERT_STREQ("both", std::string(ag::trim("  \n\t\t both \n  \t\t\n")).c_str());
}

TEST(TokenTests, ExtractNext)
{
	std::string_view remaining = "tok1 \t\n  tok2   \t\n\n  tok3";

	auto token = ag::extractNextToken(remaining);
	ASSERT_STREQ("tok1", std::string(token).c_str());
	ASSERT_STREQ(" \t\n  tok2   \t\n\n  tok3", std::string(remaining).c_str());

	token = ag::extractNextToken(remaining);
	ASSERT_STREQ("tok2", std::string(token).c_str());
	ASSERT_STREQ("   \t\n\n  tok3", std::string(remaining).c_str());

	token = ag::extractNextToken(remaining);
	ASSERT_STREQ("tok3", std::string(token).c_str());
	ASSERT_STREQ("", std::string(remaining).c_str());
}

TEST(TokenTests, ExtractNextWithOnlyWhitespace)
{
	std::string_view remaining = "   \t\n     \n\n\t  ";
	auto token = ag::extractNextToken(remaining);

	ASSERT_STREQ("", std::string(token).c_str());
	ASSERT_STREQ("   \t\n     \n\n\t  ", std::string(remaining).c_str());
}

TEST(TokenTests, ExtractUntil)
{
	std::string_view remaining = "tok1,    tok2.after   \t\n, tok3.after, tok4";

	auto token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ("tok1", std::string(token).c_str());
	ASSERT_STREQ("    tok2.after   \t\n, tok3.after, tok4", std::string(remaining).c_str());

	token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ("tok2.after", std::string(token).c_str());
	ASSERT_STREQ(" tok3.after, tok4", std::string(remaining).c_str());

	token = ag::extractUntil(remaining, '.');
	ASSERT_STREQ("tok3", std::string(token).c_str());
	ASSERT_STREQ("after, tok4", std::string(remaining).c_str());

	token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ("after", std::string(token).c_str());
	ASSERT_STREQ(" tok4", std::string(remaining).c_str());

	token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ("", std::string(token).c_str());
	ASSERT_STREQ(" tok4", std::string(remaining).c_str());
}

TEST(TokenTests, ExtractUntilWithOnlyWhitespace)
{
	std::string_view remaining = "   \t\n  \t  \n";

	auto token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ("", std::string(token).c_str());
	ASSERT_STREQ("   \t\n  \t  \n", std::string(remaining).c_str());
}
