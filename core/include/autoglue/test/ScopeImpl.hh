#ifndef AUTOGLUE_TEST_SCOPE_IMPL_HH
#define AUTOGLUE_TEST_SCOPE_IMPL_HH

#include <autoglue/test/NodeBase.hh>
#include <autoglue/Scope.hh>

namespace ag::test
{

class ScopeImpl : public AbstractScope, public NodeBase
{
public:
	ScopeImpl(std::string_view name);

	const AbstractNode& getParent() const override;
	void testSpecific(std::shared_ptr <Node> matching) const override;
};

}

#endif
