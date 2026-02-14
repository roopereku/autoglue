#include <autoglue/test/FieldImpl.hh>
#include <autoglue/test/ClassImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

FieldImpl::FieldImpl(std::string_view name)
	: AbstractField(name), NodeBase(this)
{
}

const AbstractClass& FieldImpl::getParentClass() const
{
	EXPECT_EQ(mParent.lock()->mAbstract->getType(), Node::Type::Class);
	return static_cast <AbstractClass&> (*mParent.lock()->mAbstract);
}

void FieldImpl::testSpecific(std::shared_ptr <Node> matching) const
{
	auto matchingField = matching->as <Field> ();
	ASSERT_TRUE(matchingField);

	ASSERT_TRUE(mInitializerType);
	ASSERT_TRUE(matchingField->getInitializerType());
	mInitializerType->test(*matchingField->getInitializerType());
}

const AbstractTypeUsage& FieldImpl::getInitializerType() const
{
	return *mInitializerType;
}

std::shared_ptr <FieldImpl> FieldImpl::setInitializerType(TypeUsageImpl&& usage)
{
	mInitializerType.emplace(std::move(usage));
	return std::static_pointer_cast <FieldImpl> (shared_from_this());
}

}
