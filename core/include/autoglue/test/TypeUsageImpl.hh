#ifndef AUTOGLUE_TEST_TYPE_USAGE_IMPL_HH
#define AUTOGLUE_TEST_TYPE_USAGE_IMPL_HH

#include <autoglue/AbstractTypeUsage.hh>
#include <autoglue/test/NodeBase.hh>

namespace ag::test	
{

class TypeUsageImpl : public AbstractTypeUsage
{
public:
	TypeUsageImpl(std::weak_ptr <NodeBase> decl, bool isConst, bool isReference);
	TypeUsageImpl(TypeDefinition& definition, bool isConst, bool isReference);

	void test(const TypeUsage& real) const;

	const AbstractClass& getClass() const override;
	const AbstractEnum& getEnum() const override;
	const Integer getIntegerDefinition() const override;

private:
	std::weak_ptr <NodeBase> mNode;
	TypeDefinition* mDefinition = nullptr;
};

}

#endif
