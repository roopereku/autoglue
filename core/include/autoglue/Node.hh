#ifndef AUTOGLUE_NODE_HH
#define AUTOGLUE_NODE_HH

#include <string>
#include <memory>

namespace ag
{

class NodeStorage;

class Node : public std::enable_shared_from_this <Node>
{
public:
	enum class Type
	{
		Scope,
		Class,
		Enum,
		EnumValue,
		Function,
		Parameter,
		Field
	};

	static constexpr size_t TypeCount = 6;

	/// Gets the name of this node.
	///
	/// \return The name of this node.
	const std::wstring& getName() const
	{
		return mName;
	}

	/// Gets the type of this node.
	///
	/// \return The type of this node.
	Type getType() const
	{
		return mType;
	}

	/// Finds a node with the given location within the context of this node.
	///
	/// \param location The full location of a node relative to this node.
	/// \param delimiter The character between different parts of the location.
	/// \return Node with the given location if any.
	std::shared_ptr <Node> find(std::wstring_view location, wchar_t delimiter = '.') const;

	// TODO: Add a virtual node builder function so that different node types can audit child node types.

	/// Checks if the name of this node matches the given name.
	/// Derived types can do more complex checking.
	///
	/// \param name The name to match against.
	/// \return True if the name matches.
	virtual bool matchName(std::wstring_view name) const
	{
		return name == mName;
	}

	template <typename T>
	std::shared_ptr <T> as()
	{
		if (T::matchType(mType))
		{
			return std::static_pointer_cast <T> (shared_from_this());
		}

		return nullptr;
	}

	NodeStorage& getStorage();

protected:
	Node(std::wstring&& name, Type type);
	Node(std::wstring&& name, Type type, NodeStorage& storage);

private:
	std::wstring mName;
	Type mType;

	NodeStorage& mStorage;
};

}

#endif
