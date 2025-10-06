#include <autoglue/Tree.hh>
#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Class.hh>
#include <autoglue/Enum.hh>

#include <cassert>

namespace ag
{

Tree::Tree()
{
}

std::shared_ptr <Scope> Tree::build()
{
	mGlobal = std::make_shared <Scope> (L"");

	if (!onBuild())
	{
		mGlobal = nullptr;
	}

	return mGlobal;
}

std::shared_ptr <Node> Tree::buildHierarchy(const AbstractNode& node)
{
	if (auto result = buildHierarchyRecursive(node))
	{
		if (auto function = result->as <Function> ())
		{
			function->enforceParameterMatching();
		}

		return result;
	}

	return nullptr;
}

std::shared_ptr <Node> Tree::buildHierarchyRecursive(const AbstractNode& node)
{
	if (node.isGlobalScope())
	{
		return mGlobal;
	}

	auto parent = buildHierarchyRecursive(node.getParent());
	assert(parent);

	assert(parent->getStorage().canStore(node.getType()));
	auto ensured = parent->getStorage().ensure(node.getType(), node.getName());

	if (ensured)
	{
		if (auto function = ensured->as <Function> ())
		{
			if (!function->getReturnType())
			{
				auto& returnType = node.getFunctionReturnType();
				auto& definition = ensureTypeDefinitionExists(returnType);
				function->initializeReturnType(TypeUsage(definition, returnType));
			}
		}

		if (auto variable = ensured->as <Variable> ())
		{
			if (!variable->getInitializerType())
			{
				auto& initializerType = node.getVariableInitializerType();
				auto& definition = ensureTypeDefinitionExists(initializerType);
				variable->initializeUsedType(TypeUsage(definition, initializerType));
			}
		}
	}

	return ensured;
}

TypeDefinition& Tree::ensureTypeDefinitionExists(const AbstractTypeUsage& usage)
{
	switch (usage.getTypeOfUsedDefinition())
	{
		case TypeDefinition::Type::Class:
		{
			auto node = buildHierarchy(usage.getDeclarationOfUsed())->as <Class> ();
			assert(node);
			return *node;
		}

		case TypeDefinition::Type::Enum:
		{
			auto node = buildHierarchy(usage.getDeclarationOfUsed())->as <Enum> ();
			assert(node);
			return *node;
		}

		case TypeDefinition::Type::Integer:
		{
			auto definition = usage.getIntegerDefinition();
			return *findOrAddTypeDefinition(definition);
		}

		case TypeDefinition::Type::Character:
		case TypeDefinition::Type::String:
		case TypeDefinition::Type::Float:
		case TypeDefinition::Type::Callable:
		{
			TypeDefinition definition(usage.getTypeOfUsedDefinition());
			return *findOrAddTypeDefinition(definition);
		}
	}

	assert(false);
}

std::shared_ptr <TypeDefinition> Tree::findOrAddTypeDefinition(TypeDefinition& definition)
{
	auto& existing = mNonDeclarations[definition.getType()];
	for (auto& current : existing)
	{
		if (current->matches(definition))
		{
			return current;
		}
	}
	
	return existing.emplace_back(definition.copyToHeap());
}

}
