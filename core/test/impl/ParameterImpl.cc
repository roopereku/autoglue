#include <autoglue/test/ParameterImpl.hh>
#include <autoglue/test/FunctionImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

ParameterImpl::ParameterImpl(std::wstring_view name)
	: AbstractParameter(name), NodeBase(this)
{
}

const AbstractFunction& ParameterImpl::getParentFunction() const
{
	EXPECT_EQ(mParent.lock()->mAbstract->getType(), Node::Type::Function);
	return static_cast <AbstractFunction&> (*mParent.lock()->mAbstract);
}

void ParameterImpl::testSpecific(std::shared_ptr <Node> matching) const
{
	auto matchingParameter = matching->as <Parameter> ();
	ASSERT_TRUE(matchingParameter);

	ASSERT_TRUE(mInitializerType);
	ASSERT_TRUE(matchingParameter->getInitializerType());
	mInitializerType->test(*matchingParameter->getInitializerType());
}

const AbstractTypeUsage& ParameterImpl::getInitializerType() const
{
	return *mInitializerType;
}

std::shared_ptr <ParameterImpl> ParameterImpl::setInitializerType(TypeUsageImpl&& usage)
{
	mInitializerType.emplace(std::move(usage));
	return std::static_pointer_cast <ParameterImpl> (shared_from_this());
}

}
