#include <autoglue/Tree.hh>
#include <autoglue/Class.hh>
#include <autoglue/Scope.hh>
#include <autoglue/Enum.hh>
#include <autoglue/Function.hh>

#include <cassert>

namespace ag
{

Tree::Tree()
{
}

std::shared_ptr <Scope> Tree::build()
{
	mGlobal = std::make_shared <Scope> (L"");

	if (!onBuild())
	{
		mGlobal = nullptr;
	}

	return mGlobal;
}

std::shared_ptr <Node> Tree::buildHierarchy(AbstractNode& node)
{
	auto parent = node.hasParent() ? buildHierarchy(node.getParent()) : mGlobal;
	assert(parent);

	assert(parent->getStorage().canStore(node.getType()));
	return parent->getStorage().ensure(node.getType(), node.getName());
}

}
