#include <autoglue/TypeUsage.hh>
#include <autoglue/Token.hh>

namespace ag
{

bool TypeUsage::matchName(std::string_view name) const
{
	if (mConst)
	{
		auto token = extractNextToken(name);
		if (token != "const")
		{
			return false;
		}
	}

	if (mReference)
	{
		auto token = extractNextToken(name);
		if (token != "ref")
		{
			return false;
		}
	}

	return mUsedType.matchTypeName(trim(name));
}

}
