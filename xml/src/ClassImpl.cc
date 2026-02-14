#include <autoglue/xml/ClassImpl.hh>
#include <autoglue/xml/TypeUsageImpl.hh>

namespace ag::xml
{

size_t ClassImpl::getBaseTypeCount() const
{
	return mElement.ChildElementCount("BaseType");
}

std::shared_ptr <AbstractTypeUsage> ClassImpl::getBaseType(size_t index) const
{
	size_t hops = 0;
	auto* current = mElement.FirstChildElement("BaseType");

	while (hops < index)
	{
		current = current->NextSiblingElement("BaseType");
		hops++;
	}

	return std::make_shared <TypeUsageImpl> (*current);
}

}
