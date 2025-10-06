#include <autoglue/Integer.hh>

namespace ag
{

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

}
