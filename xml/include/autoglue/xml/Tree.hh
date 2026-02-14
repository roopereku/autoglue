#ifndef AUTOGLUE_XML_TREE_HH
#define AUTOGLUE_XML_TREE_HH

#include <autoglue/Tree.hh>
#include <autoglue/Class.hh>

namespace ag::xml
{

class ClassImpl;

class Tree : public ag::Tree
{
public:
	Tree(std::string_view xmlPath)
		: mPath(xmlPath)
	{
	}

	std::shared_ptr <Class> build(ClassImpl& impl);

protected:
	bool onBuild() override;

private:
	std::string mPath;
};

}

#endif
