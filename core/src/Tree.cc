#include <autoglue/Tree.hh>
#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>

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

std::shared_ptr <Node> Tree::buildHierarchy(const AbstractNode& node)
{
	auto parent = node.hasParent() ? buildHierarchy(node.getParent()) : mGlobal;
	assert(parent);

	assert(parent->getStorage().canStore(node.getType()));
	auto ensured = parent->getStorage().ensure(node.getType(), node.getName());

	if (ensured)
	{
		if (ensured->getType() == Node::Type::Function)
		{
			auto function = ensured->as <Function> ();
			if (!function->getReturnType())
			{
				//function->initializeReturnType(
			}
		}

		else if (ensured->getType() == Node::Type::Parameter)
		{
			auto parameter = ensured->as <Parameter> ();
		}
	}

	return ensured;
}

}
