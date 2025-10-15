#include <autoglue/xml/Visitor.hh>

namespace ag::xml
{

bool Visitor::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr)
{
	//element.Parent()->Parent

	return true;
}

bool Visitor::VisitExit(const tinyxml2::XMLElement& element)
{
	return true;
}

}
