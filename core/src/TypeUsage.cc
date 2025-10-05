#include <autoglue/TypeUsage.hh>
#include <autoglue/Class.hh>
#include <autoglue/Enum.hh>

namespace ag
{

TypeUsage::TypeUsage(std::shared_ptr <Class> node)
	: mUsedType(*node)
{
}

TypeUsage::TypeUsage(std::shared_ptr <Enum> node)
	: mUsedType(*node)
{
}

}
