#include <autoglue/xml/Tree.hh>
#include <autoglue/xml/Visitor.hh>

#include <tinyxml2.h>

namespace ag::xml
{

bool Tree::onBuild()
{
	tinyxml2::XMLDocument document;
	if (document.LoadFile(mPath.c_str()) != tinyxml2::XML_SUCCESS)
	{
		return false;
	}

	auto* root = document.RootElement();
	if (!root)
	{
		return false;
	}

	Visitor visitor;
	document.Accept(&visitor);

	return true;
}

}
