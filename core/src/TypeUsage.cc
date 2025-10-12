#include <autoglue/TypeUsage.hh>
#include <autoglue/Token.hh>

namespace ag
{

bool TypeUsage::matchName(std::wstring_view name) const
{
	if (mConst)
	{
		auto token = extractNextToken(name);
		if (token != L"const")
		{
			return false;
		}
	}

	if (mReference)
	{
		auto token = extractNextToken(name);
		if (token != L"ref")
		{
			return false;
		}
	}

	return mUsedType.matchTypeName(trim(name));
}

}
