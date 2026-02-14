#include <autoglue/Integer.hh>

namespace ag
{

Integer::Integer(size_t sizeBytes, bool isUnsigned) :
	TypeDefinition(Type::Integer),
	mSizeBytes(sizeBytes), mUnsigned(isUnsigned),
	mFullName((mUnsigned ? "uint" : "int") + std::to_string(mSizeBytes * 8))
{
}

bool Integer::matches(const TypeDefinition& other) const
{
	if(!TypeDefinition::matches(other))
	{
		return false;
	}

	auto& otherInteger = static_cast <const Integer&> (other);
	return mSizeBytes == otherInteger.mSizeBytes && mUnsigned == otherInteger.mUnsigned;
}

std::shared_ptr <TypeDefinition> Integer::copyToHeap() const
{
	return std::make_shared <Integer> (mSizeBytes, mUnsigned);
}

bool Integer::matchTypeName(std::string_view name) const
{
	return name == mFullName;
}

}
