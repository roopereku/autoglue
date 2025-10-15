#include <autoglue/SignatureHolder.hh>
#include <autoglue/Token.hh>

namespace ag
{

void SignatureHolder::initializeReturnType(TypeUsage&& usage)
{
	if (!mReturnType)
	{
		mReturnType.emplace(std::move(usage));
	}
}

bool SignatureHolder::matchParameterTypes(std::string_view block) const
{
	const size_t paramCount = getParameterCount();
	for (size_t i = 0; i < paramCount; i++)
	{
		// No more parameters to compare. Mismatch in given parameters.
		if (block.empty())
		{
			return false;
		}

		// Parameter types are separated by commas.
		auto token = extractUntil(block, ',');

		// We're at the last parameter type if there's no more commas.
		if (token.empty())
		{
			token = trim(block);
			block = "";
		}

		// If the type usage held in the current token doesn't match, the given parameters don't match.
		if (!getParameterType(i)->matchName(token))
		{
			return false;
		}
	}

	// If there's still something in the parameter block, the parameter counts mismatch.
	if (!block.empty())
	{
		return false;
	}

	return true;
}

}
