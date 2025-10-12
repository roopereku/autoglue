#ifndef AUTOGLUE_XML_TREE_HH
#define AUTOGLUE_XML_TREE_HH

#include <autoglue/Tree.hh>

namespace ag::xml
{

class Tree : public ag::Tree
{
public:
	Tree(std::string_view xmlPath)
		: mPath(xmlPath)
	{
	}

protected:
	bool onBuild() override;

private:
	std::string mPath;
};

}

#endif
