#ifndef AUTOGLUE_TEST_TREE_HH
#define AUTOGLUE_TEST_TREE_HH

#include <autoglue/Tree.hh>
#include <autoglue/test/NodeBase.hh>

namespace ag::test
{

class Tree : public ag::Tree
{
public:
	Tree(std::shared_ptr <NodeBase>&& root);

	bool onBuild() override;
	void traverse(std::shared_ptr <NodeBase> node);

	std::shared_ptr <NodeBase> mRoot;

};

}

#endif
