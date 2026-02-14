#include <autoglue/test/Tree.hh>
#include <autoglue/test/FunctionImpl.hh>
#include <autoglue/test/ClassImpl.hh>
#include <autoglue/test/EnumImpl.hh>
#include <autoglue/test/FieldImpl.hh>

namespace ag::test
{

Tree::Tree(std::shared_ptr <NodeBase>&& root)
	: mRoot(root)
{
}

bool Tree::onBuild()
{
	traverse(mRoot);
	return true;
}

void Tree::traverse(std::shared_ptr <NodeBase> node)
{
	switch(node->mAbstract->getType())
	{
		case Node::Type::Function:
		{
			build(*std::static_pointer_cast <FunctionImpl> (node));
			break;
		}

		case Node::Type::Enum:
		{
			build(*std::static_pointer_cast <EnumImpl> (node));
			break;
		}

		case Node::Type::Class:
		{
			build(*std::static_pointer_cast <ClassImpl> (node));
			break;
		}

		case Node::Type::Field:
		{
			build(*std::static_pointer_cast <FieldImpl> (node));
			break;
		}

		default:
		{
		}
	}

	for (auto& inner : node->mInner)
	{
		traverse(inner);
	}
}

}
