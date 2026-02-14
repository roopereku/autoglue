#ifndef AUTOGLUE_NODE_HH
#define AUTOGLUE_NODE_HH

#include <string>
#include <memory>

namespace ag
{

class NodeStorage;

/// Node is an entry in the simplified hierarchy.
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

	static constexpr size_t TypeCount = 7;

	/// Gets the name of this node.
	///
	/// \return The name of this node.
	const std::string& getName() const
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
	std::shared_ptr <Node> find(std::string_view location, wchar_t delimiter = '.') const;

	// TODO: Add a virtual node builder function so that different node types can audit child node types.

	/// Checks if the name of this node matches the given name.
	/// Derived types can do more complex checking.
	///
	/// \param name The name to match against.
	/// \return True if the name matches.
	virtual bool matchName(std::string_view name) const
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
	Node(std::string_view name, Type type);
	Node(std::string_view name, Type type, NodeStorage& storage);

private:
	std::string mName;
	Type mType;

	NodeStorage& mStorage;
};

/// AbstractNode is used by source language specific tree building
/// code to provide a temporary source of information about a language specific node.
class AbstractNode
{
public:
	const std::string& getName() const
	{
		return mName;
	}

	Node::Type getType() const
	{
		return mType;
	}

	// TODO: Refactor this to more speficic versions where the parent has a known type?
	virtual const AbstractNode& getParent() const = 0;

	/// Determines whether this abstract node represents the global scope.
	/// This must return true for the final parent of all abstract nodes.
	///
	/// \return True if this abstract node represents the global scope.ö
	bool isGlobalScope() const
	{
		return mType == Node::Type::Scope && mName.empty();
	}

protected:
	AbstractNode(Node::Type type, std::string_view name)
		: mType(type), mName(name)
	{
	}

private:
	Node::Type mType;
	std::string mName;
};

}

#endif
