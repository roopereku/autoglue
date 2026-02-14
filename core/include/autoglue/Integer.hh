#ifndef AUTOGLUE_INTEGER_HH
#define AUTOGLUE_INTEGER_HH

#include <autoglue/TypeDefinition.hh>

#include <string>

namespace ag
{

class Integer : public TypeDefinition
{
public:
	Integer(size_t sizeBytes, bool isUnsigned);

	bool matches(const TypeDefinition& other) const override;
	std::shared_ptr <TypeDefinition> copyToHeap() const override;

	bool matchTypeName(std::string_view name) const override;

private:
	size_t mSizeBytes;
	bool mUnsigned;

	std::string mFullName;
};

}

#endif
