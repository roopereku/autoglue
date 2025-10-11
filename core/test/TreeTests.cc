#include <autoglue/test/Tree.hh>
#include <autoglue/test/ScopeImpl.hh>
#include <autoglue/test/ClassImpl.hh>
#include <autoglue/test/FunctionImpl.hh>
#include <autoglue/test/EnumImpl.hh>
#include <autoglue/test/ParameterImpl.hh>
#include <autoglue/test/FieldImpl.hh>

#include <gtest/gtest.h>

using namespace ag;
using namespace ag::test;

TEST(TreeTests, ExclusivelyScopesBuildsNothing)
{
	test::Tree tree(
		makeNode <ScopeImpl> (L"", {
			makeNode <ScopeImpl> (L"Scope1", {
				makeNode <ScopeImpl> (L"Scope1_1"),
				makeNode <ScopeImpl> (L"Scope1_2")
			}),

			makeNode <ScopeImpl> (L"Scope2", {
				makeNode <ScopeImpl> (L"Scope2_1")
			})
		})
	);

	auto root = tree.build();
	ASSERT_EQ(root->children.begin(), root->children.end());
}

TEST(TreeTests, BuildClassWithFields)
{
	Integer uint64Definition(8, true);

	test::Tree tree(
		makeNode <ScopeImpl> (L"", {
			makeNode <ClassImpl> (L"foo", {
				makeNode <FieldImpl> (L"field1")
					->setInitializerType(TypeUsageImpl(uint64Definition, false, false)),

				makeNode <FieldImpl> (L"field2")
					->setInitializerType(TypeUsageImpl(uint64Definition, false, false)),

				makeNode <FieldImpl> (L"field3")
					->setInitializerType(TypeUsageImpl(uint64Definition, false, false)),
			}),
		})
	);

	auto root = tree.build();
	testNode(tree.mRoot, root);
}
