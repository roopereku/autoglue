#include <autoglue/TypeUsage.hh>
#include <autoglue/Class.hh>
#include <autoglue/Enum.hh>

namespace ag
{

bool TypeUsage::matchName(std::wstring_view name) const
{
	// TODO: Match modifiers.

	return mUsedType.matchTypeName(name);
}

}
