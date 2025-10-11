#ifndef AUTOGLUE_TEST_FIELD_IMPL_HH
#define AUTOGLUE_TEST_FIELD_IMPL_HH

#include <autoglue/test/NodeBase.hh>
#include <autoglue/test/TypeUsageImpl.hh>
#include <autoglue/Field.hh>

namespace ag::test
{

class FieldImpl : public AbstractField, public NodeBase
{
public:
	FieldImpl(std::wstring&& name);

	const AbstractClass& getParentClass() const override;
	void testSpecific(std::shared_ptr <Node> matching) const override;

	const AbstractTypeUsage& getInitializerType() const override;
	std::shared_ptr <FieldImpl> setInitializerType(TypeUsageImpl&& usage);

private:
	std::optional <TypeUsageImpl> mInitializerType;
};

}

#endif
