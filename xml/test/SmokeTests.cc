#include <autoglue/xml/Tree.hh>

#include <gtest/gtest.h>

TEST(SmokeTests, InvalidPathDoesNothing)
{
	ag::xml::Tree tree("invalid_file");
	ASSERT_FALSE(tree.build());
}
