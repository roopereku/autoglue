#ifndef AUTOGLUE_TEST_FUNCTION_IMPL_HH
#define AUTOGLUE_TEST_FUNCTION_IMPL_HH

#include <autoglue/test/NodeBase.hh>
#include <autoglue/test/TypeUsageImpl.hh>
#include <autoglue/Function.hh>

namespace ag::test
{

class FunctionImpl : public AbstractFunction, public NodeBase
{
public:
	FunctionImpl(std::wstring&& name);

	const AbstractNode& getParent() const override;
	void testSpecific(std::shared_ptr <Node> matching) const override;

	const AbstractTypeUsage& getReturnType() const override;
	std::shared_ptr <FunctionImpl> setReturnType(TypeUsageImpl&& usage);

	size_t getParameterCount() const override;
	const AbstractParameter& getParameter(size_t index) const override;

private:
	std::optional <TypeUsageImpl> mReturnType;
};

}

#endif
