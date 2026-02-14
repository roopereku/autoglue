#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/Token.hh>

namespace ag
{

Node::Node(std::string_view name, Type type)
	: Node(name, type, NodeStorage::getDefault())
{
}

Node::Node(std::string_view name, Type type, NodeStorage& storage)
	: mName(name), mType(type), mStorage(storage)
{
}

std::shared_ptr <Node> Node::find(std::string_view location, wchar_t delimiter) const
{
	bool foundDelimiter;
	auto token = extractUntilOrNextToken(location, delimiter, foundDelimiter);

	if (auto node = mStorage.getNodeByName(token))
	{
		// If there was no delimiter, look no further.
		if (!foundDelimiter)
		{
			return node;
		}

		return node->find(location);
	}

	return nullptr;
}

NodeStorage& Node::getStorage()
{
	return mStorage;
}

}
