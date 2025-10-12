#include <autoglue/test/EnumImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

EnumImpl::EnumImpl(std::wstring_view name)
	: AbstractEnum(name), NodeBase(this)
{
}

const AbstractNode& EnumImpl::getParent() const
{
	return *mParent.lock()->mAbstract;
}

void EnumImpl::testSpecific(std::shared_ptr <Node> matching) const
{
	auto matchingEnum = matching->as <Enum> ();
	ASSERT_TRUE(matchingEnum);

	// TODO: Test value type.
}

}
