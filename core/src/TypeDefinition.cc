#include <autoglue/TypeDefinition.hh>

namespace ag
{

bool TypeDefinition::matches(const TypeDefinition& other) const
{
	return mType == other.mType;
}

std::shared_ptr <TypeDefinition> TypeDefinition::copyToHeap() const
{
	return std::make_shared <TypeDefinition> (mType);
}

}
