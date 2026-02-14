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

	void initializeReturnType(TypeUsage&& usage);

	/// Checks if the given name matches the parameter types.
	///
	/// \param block String containing the parameter types. Expected format is "mod type1, mod type2".
	/// \return True if the parameter types match.
	bool matchParameterTypes(std::string_view block) const;

private:
	std::optional <TypeUsage> mReturnType;
};

}

#endif
