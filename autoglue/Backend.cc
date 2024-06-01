#include <autoglue/Backend.hh>

namespace ag
{

Backend::Backend(std::shared_ptr <Entity>&& root)
	: root(std::move(root))
{
}

std::shared_ptr <Entity> Backend::getRootPtr()
{
	return root;
}

Entity& Backend::getRoot()
{
	return *root;
}

void Backend::ensureGlueGenerated()
{
	if(!glueGenerated)
	{
		glueGenerated = true;
		generateGlue();
	}
}

}
