#include <autoglue/test/FunctionImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

FunctionImpl::FunctionImpl(std::wstring_view name)
	: AbstractFunction(name), NodeBase(this)
{
}

const AbstractNode& FunctionImpl::getParent() const
{
	return *mParent.lock()->mAbstract;
}

void FunctionImpl::testSpecific(std::shared_ptr <Node> matching) const
{
	auto matchingFunction = matching->as <Function> ();
	ASSERT_TRUE(matchingFunction);

	ASSERT_TRUE(mReturnType);
	ASSERT_TRUE(matchingFunction->getReturnType());
	mReturnType->test(*matchingFunction->getReturnType());
}

const AbstractTypeUsage& FunctionImpl::getReturnType() const
{
	return *mReturnType;
}

std::shared_ptr <FunctionImpl> FunctionImpl::setReturnType(TypeUsageImpl&& usage)
{
	mReturnType.emplace(std::move(usage));
	return std::static_pointer_cast <FunctionImpl> (shared_from_this());
}

size_t FunctionImpl::getParameterCount() const
{
	return mInner.size();
}

const AbstractParameter& FunctionImpl::getParameter(size_t index) const
{
	auto param = mInner[index]->mAbstract;
	EXPECT_TRUE(param->getType() == Node::Type::Parameter);
	return static_cast <AbstractParameter&> (*param);
}

}
