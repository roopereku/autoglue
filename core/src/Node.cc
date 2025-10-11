#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/Token.hh>

namespace ag
{

Node::Node(std::wstring&& name, Type type)
	: Node(std::move(name), type, NodeStorage::getDefault())
{
}

Node::Node(std::wstring&& name, Type type, NodeStorage& storage)
	: mName(std::move(name)), mType(type), mStorage(storage)
{
}

std::shared_ptr <Node> Node::find(std::wstring_view location, wchar_t delimiter) const
{
	auto token = extractUntil(location, delimiter);
	bool noDelimiter = token.empty();

	if (noDelimiter)
	{
		token = trim(location);
	}

	if (auto node = mStorage.getNodeByName(token))
	{
		// If there was no delimiter, look no further.
		if (noDelimiter)
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
