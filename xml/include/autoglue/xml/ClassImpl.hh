#ifndef AUTOGLUE_XML_CLASS_IMPL
#define AUTOGLUE_XML_CLASS_IMPL

#include <autoglue/Class.hh>
#include <autoglue/xml/NodeInfo.hh>

namespace ag::xml
{

class ClassImpl : public AbstractClass
{
public:
	ClassImpl(NodeInfo& info)
		: AbstractClass(*info.getName()), mInfo(info)
	{
	}

	size_t getBaseTypeCount() const override;
	std::shared_ptr <AbstractTypeUsage> getBaseType(size_t index) const override;

private:
	NodeInfo& mInfo;
};

}

#endif
