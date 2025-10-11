#include <autoglue/Tree.hh>
#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Class.hh>
#include <autoglue/Enum.hh>
#include <autoglue/Field.hh>

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

// TODO: Check validity of result here and below.

std::shared_ptr <Function> Tree::build(const AbstractFunction& node)
{
	auto result = buildHierarchyRecursive(node)->as <Function> ();
	const size_t parameterCount = node.getParameterCount();

	for (size_t i = 0; i < parameterCount; i++)
	{
		const auto& parameter = node.getParameter(i);

		// TODO: Rename the parameter if the name already existed?
		if (auto ensured = result->parameters.ensure(parameter.getType(), parameter.getName()))
		{
			const auto& abstractInitializer = parameter.getInitializerType();
			auto& usedType = ensureTypeDefinitionExists(abstractInitializer);
			ensured->as <Parameter> ()->initializeUsedType(TypeUsage(usedType, abstractInitializer));
		}
	}

	return result;
}

std::shared_ptr <Class> Tree::build(const AbstractClass& node)
{
	auto result = buildHierarchyRecursive(node);
	return result->as <Class> ();
}

std::shared_ptr <Field> Tree::build(const AbstractField& node)
{
	auto result = buildHierarchyRecursive(node);
	return result->as <Field> (); }

std::shared_ptr <Enum> Tree::build(const AbstractEnum& node)
{
	auto result = buildHierarchyRecursive(node);
	return result->as <Enum> ();
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

	return ensured;
}

TypeDefinition& Tree::ensureTypeDefinitionExists(const AbstractTypeUsage& usage)
{
	switch (usage.getTypeOfUsedDefinition())
	{
		case TypeDefinition::Type::Class:
		{
			auto node = build(usage.getClass())->as <Class> ();
			assert(node);
			return *node;
		}

		case TypeDefinition::Type::Enum:
		{
			auto node = build(usage.getEnum())->as <Enum> ();
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
		case TypeDefinition::Type::Void:
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
