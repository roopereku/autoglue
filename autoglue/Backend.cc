#include <autoglue/Backend.hh>

#include <cassert>

namespace ag
{

Backend::Backend(std::shared_ptr <Entity>&& root)
	: root(std::move(root))
{
}

Backend::Backend(std::unique_ptr <Deserializer>&& deserializer)
	: deserializer(std::move(deserializer))
{
	// TODO: Initialize root and deserialize.
}

std::shared_ptr <Entity> Backend::getRootPtr()
{
	return root;
}

Entity& Backend::getRoot()
{
	return *root;
}

bool Backend::generateHierarchy()
{
	if(deserializer)
	{
		// TODO: Deserialize the hierarchy.
		assert(false && "TODO: Deserialize the hierarchy");
		return false;
	}

	else
	{
		return onGenerateHierarchy();
	}
}

void Backend::ensureGlueGenerated()
{
	if(!glueGenerated)
	{
		glueGenerated = true;
		onGenerateGlue();
	}
}

}
