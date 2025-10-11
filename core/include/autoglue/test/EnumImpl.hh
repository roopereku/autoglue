#ifndef AUTOGLUE_TEST_ENUM_IMPL_HH
#define AUTOGLUE_TEST_ENUM_IMPL_HH

#include <autoglue/test/NodeBase.hh>
#include <autoglue/Enum.hh>

namespace ag::test
{

class EnumImpl : public AbstractEnum, public NodeBase
{
public:
	EnumImpl(std::wstring&& name);

	const AbstractNode& getParent() const override;
	void testSpecific(std::shared_ptr <Node> matching) const override;
};

}

#endif
