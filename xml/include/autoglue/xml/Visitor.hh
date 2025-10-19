#ifndef AUTOGLUE_XML_VISITOR_HH
#define AUTOGLUE_XML_VISITOR_HH

#include <autoglue/xml/Tree.hh>

#include <tinyxml2.h>

namespace ag::xml
{

class Visitor : public tinyxml2::XMLVisitor
{
public:
	Visitor(Tree& tree)
		: mTree(tree)
	{
	}

	bool VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr) override;

private:
	Tree& mTree;
};

}

#endif
