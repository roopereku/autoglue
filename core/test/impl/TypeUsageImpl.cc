#include <autoglue/test/TypeUsageImpl.hh>
#include <autoglue/test/ClassImpl.hh>
#include <autoglue/test/EnumImpl.hh>

#include <gtest/gtest.h>

namespace ag::test
{

TypeUsageImpl::TypeUsageImpl(std::weak_ptr <NodeBase> decl, bool isConst, bool isReference)
	: AbstractTypeUsage(decl.lock()->whichTypeDefinition()), mNode(decl)
{
	mConst = isConst;
	mReference = isReference;
}

TypeUsageImpl::TypeUsageImpl(TypeDefinition& definition, bool isConst, bool isReference)
	: AbstractTypeUsage(definition.getType()), mDefinition(&definition)
{
	mConst = isConst;
	mReference = isReference;
}

void TypeUsageImpl::test(const TypeUsage& real) const
{
	ASSERT_EQ(mConst, real.isConst());
	ASSERT_EQ(mReference, real.isReference());
	ASSERT_EQ(getTypeOfUsedDefinition(), real.getUsedType().getType());
}

const AbstractClass& TypeUsageImpl::getClass() const
{
	EXPECT_FALSE(mNode.expired());
	return *std::static_pointer_cast <ClassImpl> (mNode.lock());
}

const AbstractEnum& TypeUsageImpl::getEnum() const
{
	EXPECT_FALSE(mNode.expired());
	return *std::static_pointer_cast <EnumImpl> (mNode.lock());
}

const Integer TypeUsageImpl::getIntegerDefinition() const
{
	return static_cast <Integer&> (*mDefinition);
}

}
