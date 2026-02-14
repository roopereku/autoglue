#ifndef AUTOGLUE_CALLABLE_HH
#define AUTOGLUE_CALLABLE_HH

#include <autoglue/TypeDefinition.hh>
#include <autoglue/SignatureHolder.hh>

#include <vector>

namespace ag
{

class Callable : public TypeDefinition, public SignatureHolder
{
public:
	Callable()
		: TypeDefinition(Type::Callable)
	{
	}

	size_t getParameterCount() const override
	{
		return mParameterTypes.size();
	}

	std::optional <TypeUsage> getParameterType(size_t index) const override
	{
		return mParameterTypes[index];
	}

private:
	std::vector <TypeUsage> mParameterTypes;
};

}

#endif
