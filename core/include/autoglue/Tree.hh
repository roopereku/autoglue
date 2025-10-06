#ifndef AUTOGLUE_TREE_HH
#define AUTOGLUE_TREE_HH

#include <autoglue/AbstractNode.hh>
#include <autoglue/TypeDefinition.hh>
#include <autoglue/Scope.hh>

#include <unordered_map>

namespace ag
{

/// Tree is the base class for classes implementing
/// generation of a simplified hierarchy for source languages.
class Tree
{
public:
	Tree();

	/// Invokes the implementation defined tree building.
	///
	/// \return The global scope containing the built tree.
	std::shared_ptr <Scope> build();

protected:
	/// Called when the tree building is started.
	///
	/// \return True on success.
	virtual bool onBuild() = 0;

	/// Builds non-existing parts in the given hierarchy.
	///
	/// \param Implementation defined logic for retrieving a hierarchy.
	/// \return The first node in the given ancestor hierarchy.
	std::shared_ptr <Node> buildHierarchy(const AbstractNode& node);

private:
	TypeDefinition& ensureTypeDefinitionExists(const AbstractTypeUsage& usage);
	std::shared_ptr <TypeDefinition> findOrAddTypeDefinition(TypeDefinition& definition);

	std::shared_ptr <Scope> mGlobal;

	/// Used types that are not declarations. For example, primitives, callables.
	std::unordered_map <TypeDefinition::Type, std::vector <std::shared_ptr <TypeDefinition>>> mNonDeclarations;
};


}

#endif
