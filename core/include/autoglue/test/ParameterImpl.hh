#ifndef AUTOGLUE_TEST_PARAMETER_IMPL_HH
#define AUTOGLUE_TEST_PARAMETER_IMPL_HH

#include <autoglue/test/NodeBase.hh>
#include <autoglue/test/TypeUsageImpl.hh>
#include <autoglue/Parameter.hh>

namespace ag::test
{

class ParameterImpl : public AbstractParameter, public NodeBase
{
public:
	ParameterImpl(std::wstring&& name);

	const AbstractFunction& getParentFunction() const override;
	void testSpecific(std::shared_ptr <Node> matching) const override;

	const AbstractTypeUsage& getInitializerType() const override;
	std::shared_ptr <ParameterImpl> setInitializerType(TypeUsageImpl&& usage);

private:
	std::optional <TypeUsageImpl> mInitializerType;
};

}

#endif
