#ifndef AUTOGLUE_INTEGER_HH
#define AUTOGLUE_INTEGER_HH

#include <autoglue/TypeDefinition.hh>

namespace ag
{

class Integer : public TypeDefinition
{
public:
	Integer()
		: TypeDefinition(Type::Integer)
	{
	}

private:
	size_t mSizeBytes;
};

}

#endif
