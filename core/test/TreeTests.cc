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

TEST(TreeTests, BuildFunctionWithDifferentTypes)
{
	Integer uint64Definition(8, true);
	Integer int16Definition(2, false);

	test::Tree tree(
		makeNode <ScopeImpl> (L"", {
			makeNode <FunctionImpl> (L"foo", {
				makeNode <ParameterImpl> (L"param1"),
				makeNode <ParameterImpl> (L"param2"),
				makeNode <ParameterImpl> (L"param3")
			}),

			makeNode <ClassImpl> (L"baz")
		})
	);

	ASSERT_EQ(tree.mRoot->mInner[0]->mAbstract->getType(), Node::Type::Function);
	auto fooAbstract = std::static_pointer_cast <FunctionImpl> (tree.mRoot->mInner[0]);

	ASSERT_EQ(tree.mRoot->mInner[1]->mAbstract->getType(), Node::Type::Class);
	auto bazAbstract = std::static_pointer_cast <ClassImpl> (tree.mRoot->mInner[1]);

	// Set function return value and parameters.
	fooAbstract->setReturnType(TypeUsageImpl(bazAbstract, false, false));

	std::array <TypeUsageImpl, 3> paramTypes
	{
		TypeUsageImpl(int16Definition, false, false),
		TypeUsageImpl(uint64Definition, false, true),
		TypeUsageImpl(bazAbstract, true, true)
	};

	for (size_t i = 0; i < paramTypes.size(); i++)
	{
		ASSERT_EQ(fooAbstract->mInner[i]->mAbstract->getType(), Node::Type::Parameter);
		std::static_pointer_cast <ParameterImpl> (fooAbstract->mInner[i])->setInitializerType(std::move(paramTypes[i]));
	}

	auto root = tree.build();
	testNode(tree.mRoot, root);
}
