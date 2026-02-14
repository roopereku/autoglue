#ifndef AUTOGLUE_TEST_CLASS_IMPL_HH
#define AUTOGLUE_TEST_CLASS_IMPL_HH

#include <autoglue/test/NodeBase.hh>
#include <autoglue/test/TypeUsageImpl.hh>
#include <autoglue/Class.hh>

namespace ag::test
{

class ClassImpl : public AbstractClass, public NodeBase
{
public:
	ClassImpl(std::string_view name);

	size_t getBaseTypeCount() const override;
	const AbstractTypeUsage& getBaseType(size_t index) const override;
	const AbstractNode& getParent() const override;
	void testSpecific(std::shared_ptr <Node> matching) const override;

private:
	std::vector <TypeUsageImpl> mBaseTypes;
};

}

#endif
