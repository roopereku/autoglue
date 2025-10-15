#ifndef AUTOGLUE_XML_CLASS_IMPL
#define AUTOGLUE_XML_CLASS_IMPL

#include <autoglue/Class.hh>

#include <tinyxml2.h>

namespace ag::xml
{

class ClassImpl : public AbstractClass
{
public:
	ClassImpl(tinyxml2::XMLElement& element)
		: AbstractClass(""), mElement(element)
	{
	}

private:
	tinyxml2::XMLElement& mElement;
};

}

#endif
