#include <autoglue/test/Generator.hh>
#include <autoglue/test/ScopeImpl.hh>
#include <autoglue/test/ClassImpl.hh>
#include <autoglue/test/EnumImpl.hh>
#include <autoglue/test/FunctionImpl.hh>
#include <autoglue/test/ScopeImpl.hh>
#include <autoglue/test/FieldImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

Generator::Generator()
{
	mCurrent = std::make_shared <ScopeImpl> (L"");
}

void Generator::testAgainst(std::shared_ptr <NodeBase> root) const
{
	ASSERT_EQ(mCurrent->mAbstract->getType(), Node::Type::Scope);
	ASSERT_STREQ(mCurrent->mAbstract->getName().c_str(), L"");

	compare(mCurrent, root);
}

void Generator::compare(std::shared_ptr <NodeBase> generated, std::shared_ptr <NodeBase> original) const
{
	ASSERT_EQ(generated->mAbstract->getType(), original->mAbstract->getType());
	ASSERT_STREQ(generated->mAbstract->getName().c_str(), original->mAbstract->getName().c_str());
}

void Generator::generateClass(std::shared_ptr <Class> node)
{
	mCurrent = mCurrent->addInner(std::make_shared <ClassImpl> (node->getName()));
	mCurrent = mCurrent->mParent.lock();
}

void Generator::generateEnum(std::shared_ptr <Enum> node)
{
	mCurrent = mCurrent->addInner(std::make_shared <EnumImpl> (node->getName()));
	mCurrent = mCurrent->mParent.lock();
}

void Generator::generateFunction(std::shared_ptr <Function> node)
{
	mCurrent = mCurrent->addInner(std::make_shared <FunctionImpl> (node->getName()));
	mCurrent = mCurrent->mParent.lock();
}

void Generator::generateScope(std::shared_ptr <Scope> node)
{
	mCurrent = mCurrent->addInner(std::make_shared <ScopeImpl> (node->getName()));
	mCurrent = mCurrent->mParent.lock();
}

void Generator::generateField(std::shared_ptr <Field> node)
{
	mCurrent = mCurrent->addInner(std::make_shared <FieldImpl> (node->getName()));
	mCurrent = mCurrent->mParent.lock();
}

}
