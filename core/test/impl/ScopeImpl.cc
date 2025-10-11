#include <autoglue/test/ScopeImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

ScopeImpl::ScopeImpl(std::wstring&& name)
	: AbstractScope(std::move(name)), NodeBase(this)
{
}

const AbstractNode& ScopeImpl::getParent() const
{
	return *mParent.lock()->mAbstract;
}

void ScopeImpl::testSpecific(std::shared_ptr <Node> matching) const
{
	auto matchingScope = matching->as <Scope> ();
	ASSERT_TRUE(matchingScope);
}

}
