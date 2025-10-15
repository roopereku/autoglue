#ifndef AUTOGLUE_XML_VISITOR_HH
#define AUTOGLUE_XML_VISITOR_HH

#include <tinyxml2.h>

namespace ag::xml
{

class Visitor : public tinyxml2::XMLVisitor
{
public:
	bool VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr) override;
	bool VisitExit(const tinyxml2::XMLElement& element) override;
};

}

#endif
