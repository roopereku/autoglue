#ifndef AUTOGLUE_INTEGER_HH
#define AUTOGLUE_INTEGER_HH

#include <autoglue/TypeDefinition.hh>

namespace ag
{

class Integer : public TypeDefinition
{
public:
	Integer(size_t sizeBytes, bool isUnsigned)
		: TypeDefinition(Type::Integer), mSizeBytes(sizeBytes), mUnsigned(isUnsigned)
	{
	}

	bool matches(const TypeDefinition& other) const override;
	std::shared_ptr <TypeDefinition> copyToHeap() const override;

private:
	size_t mSizeBytes;
	bool mUnsigned;
};

}

#endif
