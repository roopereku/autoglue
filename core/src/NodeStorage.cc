#include <autoglue/NodeStorage.hh>
#include <autoglue/Enum.hh>
#include <autoglue/Class.hh>
#include <autoglue/Scope.hh>
#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Field.hh>
#include <autoglue/EnumValue.hh>

#include <cassert>

namespace ag
{

NodeStorage NodeStorage::withAllTypes()
{
	std::bitset <Node::TypeCount> allowed;
	allowed.set();

	return NodeStorage(std::move(allowed));
}

NodeStorage NodeStorage::withTypes(std::initializer_list <Node::Type> included)
{
	std::bitset <Node::TypeCount> allowed;
	for (auto exclusion : included)
	{
		allowed.set(static_cast <size_t> (exclusion));
	}

	return NodeStorage(std::move(allowed));

}

NodeStorage NodeStorage::withAllTypesExcept(std::initializer_list <Node::Type> excluded)
{
	auto storage = withAllTypes();
	for (auto exclusion : excluded)
	{
		storage.mAllowedTypes.reset(static_cast <size_t> (exclusion));
	}

	return storage;
}

std::shared_ptr <Node> NodeStorage::getNodeByName(std::string_view name) const
{
	for (const auto& node : mNodes)
	{
		if (node->matchName(name))
		{
			return node;
		}
	}

	return nullptr;
}

std::shared_ptr <Node> NodeStorage::getNodeByName(std::string_view name, Node::Type type) const
{
	for (const auto& node : mNodes)
	{
		if (node->getType() == type && node->matchName(name))
		{
			return node;
		}
	}

	return nullptr;
}

std::shared_ptr <Node> NodeStorage::ensure(Node::Type type, std::string_view name)
{
	if (!canStore(type))
	{
		return nullptr;
	}

	if (auto existing = getNodeByName(name, type))
	{
		return existing;
	}

	std::shared_ptr <Node> node;
	switch (type)
	{
		case Node::Type::Class: node = std::make_shared <Class> (std::string(name)); break;
		case Node::Type::Scope: node = std::make_shared <Scope> (std::string(name)); break;
		case Node::Type::Enum: node = std::make_shared <Enum> (std::string(name)); break;
		case Node::Type::EnumValue: node = std::make_shared <EnumValue> (std::string(name)); break;
		case Node::Type::Parameter: node = std::make_shared <Parameter> (std::string(name)); break;
		case Node::Type::Field: node = std::make_shared <Field> (std::string(name)); break;
		case Node::Type::Function: node = std::make_shared <Function> (std::string(name)); break;
	}

	return mNodes.emplace_back(std::move(node));
}

bool NodeStorage::canStore(Node::Type type) const
{
	return mAllowedTypes.test(static_cast <size_t> (type));
}

bool NodeStorage::isEmpty() const
{
	return mNodes.empty();
}

NodeStorage& NodeStorage::getDefault()
{
	static auto storage = withTypes({});
	return storage;
}

}
