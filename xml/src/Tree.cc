#include <autoglue/xml/Tree.hh>

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

	return true;
}

}
