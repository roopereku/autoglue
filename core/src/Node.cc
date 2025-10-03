#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>

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
	size_t delimiterAt = location.find(delimiter);
	const auto currentName = location.substr(0, std::min(delimiterAt, location.size()));

	if (auto node = mStorage.getNodeByName(currentName))
	{
		// If there was no delimiter, look no further.
		if (delimiterAt == std::wstring_view::npos)
		{
			return node;
		}

		delimiterAt++;
		const auto remaining = location.substr(delimiterAt, location.size() - delimiterAt);

		return node->find(remaining);
	}

	return nullptr;
}

NodeStorage& Node::getStorage()
{
	return mStorage;
}

}
