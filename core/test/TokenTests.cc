#include <autoglue/Token.hh>

#include <gtest/gtest.h>

#include <string>

TEST(TokenTests, TrimLeft)
{
	ASSERT_STREQ(L"no left trim", std::wstring(ag::trimLeft(L"no left trim")).c_str());
	ASSERT_STREQ(L"left trim", std::wstring(ag::trimLeft(L"  \t\n\t\n  left trim")).c_str());
}

TEST(TokenTests, TrimRight)
{
	ASSERT_STREQ(L"no right trim", std::wstring(ag::trimRight(L"no right trim")).c_str());
	ASSERT_STREQ(L"right trim", std::wstring(ag::trimRight(L"right trim\t \n  \n\t  ")).c_str());
}

TEST(TokenTests, TrimBoth)
{
	ASSERT_STREQ(L"left but no right", std::wstring(ag::trim(L"      left but no right")).c_str());
	ASSERT_STREQ(L"right but no left", std::wstring(ag::trim(L"right but no left        \n\t")).c_str());
	ASSERT_STREQ(L"both", std::wstring(ag::trim(L"  \n\t\t both \n  \t\t\n")).c_str());
}

TEST(TokenTests, ExtractNext)
{
	std::wstring_view remaining = L"tok1 \t\n  tok2   \t\n\n  tok3";

	auto token = ag::extractNextToken(remaining);
	ASSERT_STREQ(L"tok1", std::wstring(token).c_str());
	ASSERT_STREQ(L" \t\n  tok2   \t\n\n  tok3", std::wstring(remaining).c_str());

	token = ag::extractNextToken(remaining);
	ASSERT_STREQ(L"tok2", std::wstring(token).c_str());
	ASSERT_STREQ(L"   \t\n\n  tok3", std::wstring(remaining).c_str());

	token = ag::extractNextToken(remaining);
	ASSERT_STREQ(L"tok3", std::wstring(token).c_str());
	ASSERT_STREQ(L"", std::wstring(remaining).c_str());
}

TEST(TokenTests, ExtractNextWithOnlyWhitespace)
{
	std::wstring_view remaining = L"   \t\n     \n\n\t  ";
	auto token = ag::extractNextToken(remaining);

	ASSERT_STREQ(L"", std::wstring(token).c_str());
	ASSERT_STREQ(L"   \t\n     \n\n\t  ", std::wstring(remaining).c_str());
}

TEST(TokenTests, ExtractUntil)
{
	std::wstring_view remaining = L"tok1,    tok2.after   \t\n, tok3.after, tok4";

	auto token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ(L"tok1", std::wstring(token).c_str());
	ASSERT_STREQ(L"    tok2.after   \t\n, tok3.after, tok4", std::wstring(remaining).c_str());

	token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ(L"tok2.after", std::wstring(token).c_str());
	ASSERT_STREQ(L" tok3.after, tok4", std::wstring(remaining).c_str());

	token = ag::extractUntil(remaining, '.');
	ASSERT_STREQ(L"tok3", std::wstring(token).c_str());
	ASSERT_STREQ(L"after, tok4", std::wstring(remaining).c_str());

	token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ(L"after", std::wstring(token).c_str());
	ASSERT_STREQ(L" tok4", std::wstring(remaining).c_str());

	token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ(L"", std::wstring(token).c_str());
	ASSERT_STREQ(L" tok4", std::wstring(remaining).c_str());
}

TEST(TokenTests, ExtractUntilWithOnlyWhitespace)
{
	std::wstring_view remaining = L"   \t\n  \t  \n";

	auto token = ag::extractUntil(remaining, ',');
	ASSERT_STREQ(L"", std::wstring(token).c_str());
	ASSERT_STREQ(L"   \t\n  \t  \n", std::wstring(remaining).c_str());
}
