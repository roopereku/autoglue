#ifndef AUTOGLUE_SIGNATURE_HOLDER_HH
#define AUTOGLUE_SIGNATURE_HOLDER_HH

#include <autoglue/TypeUsage.hh>

#include <optional>

namespace ag
{

class SignatureHolder
{
public:
	virtual size_t getParameterCount() const = 0;

	virtual std::optional <TypeUsage> getParameterType(size_t index) const = 0;

	std::optional <TypeUsage> getReturnType() const
	{
		return mReturnType;
	}

	void initializeReturnType(TypeUsage&& usage)
	{
		if (!mReturnType)
		{
			mReturnType.emplace(std::move(usage));
		}
	}

private:
	std::optional <TypeUsage> mReturnType;
};

}

#endif
