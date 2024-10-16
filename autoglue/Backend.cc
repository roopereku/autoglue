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
		// Assuming that no root exists, deserialize a hierarchy.
		assert(!root);
		root = deserializer->createHierarchy();

		// Destroy the deserializer and return true if a hierarchy now exists.
		deserializer = nullptr;
		return static_cast <bool> (root);
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
