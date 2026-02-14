#include <autoglue/test/ClassImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

ClassImpl::ClassImpl(std::string_view name)
	: AbstractClass(name), NodeBase(this)
{
}

size_t ClassImpl::getBaseTypeCount() const
{
	return mBaseTypes.size();
}

const AbstractTypeUsage& ClassImpl::getBaseType(size_t index) const
{
	return mBaseTypes[index];
}

const AbstractNode& ClassImpl::getParent() const
{
	return *mParent.lock()->mAbstract;
}

void ClassImpl::testSpecific(std::shared_ptr <Node> matching) const
{
	auto matchingClass = matching->as <Class> ();
	ASSERT_TRUE(matchingClass);

	ASSERT_EQ(mBaseTypes.size(), matchingClass->getBaseTypeCount());
	for (size_t i = 0; i < mBaseTypes.size(); i++)
	{
		mBaseTypes[i].test(matchingClass->getBaseType(i));
	}
}

}
