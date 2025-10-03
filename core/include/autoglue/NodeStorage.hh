#ifndef AUTOGLUE_NODE_STORAGE_HH
#define AUTOGLUE_NODE_STORAGE_HH

#include <autoglue/Node.hh>

#include <vector>
#include <bitset>

namespace ag
{

class NodeStorage
{
public:
	/// Constructs a node storage which accepts all node types.
	///
	/// \return Node storage accepting all node types.
	static NodeStorage withAllTypes();

	/// Constructs a node storage which allows the specified node types.
	///
	/// \param included The types to allow.
	/// \return Node storage accepting the given node types.
	static NodeStorage withTypes(std::initializer_list <Node::Type> included);

	/// Constructs a node storage which accepts all node types except the given exclusions.
	///
	/// \param allowedTypes The types to exclude.
	/// \return Node storage accepting all node types excluding the given types.
	static NodeStorage withAllTypesExcept(std::initializer_list <Node::Type> excluded);

	/// Gets the first node with a matching name.
	///
	/// \param name The name to match.
	/// \return The first contained node with a matching name if any.
	std::shared_ptr <Node> getNodeByName(std::wstring_view name) const;

	/// Gets the first node with a matching name and type.
	///
	/// \param name The name to match.
	/// \param type The type of the node to get.
	/// \return The first contained node with a matching name if any.
	std::shared_ptr <Node> getNodeByName(std::wstring_view name, Node::Type type) const;

	/// Ensures that a node of the given type with the given name exists.
	///
	/// \param type The desired node type. This has to be allowed by the node storage.
	/// \param name The desired node name.
	/// \return The node of the given type and name if the type is allowed.
	std::shared_ptr <Node> ensure(Node::Type type, std::wstring_view name);

	/// Checks whether this node storage can store the given type.
	///
	/// \param type The type to check
	/// \return True if the given type can be stored.
	bool canStore(Node::Type type) const;

	/// Gets the default node storage that nothing can be added into.
	///
	/// \return NodeStorage that nothing can be added into.
	static NodeStorage& getDefault();

	auto begin() { return mNodes.begin(); }
	auto end() { return mNodes.end(); }
	auto cbegin() const { return mNodes.cbegin(); }
	auto cend() const { return mNodes.cend(); }

private:
	NodeStorage(std::bitset <Node::TypeCount>&& allowed)
		: mAllowedTypes(std::move(allowed))
	{
	}

	std::vector <std::shared_ptr <Node>> mNodes;

	/// Which node types are allowed. Bit per type. 
	std::bitset <Node::TypeCount> mAllowedTypes;
};

}

#endif
