#include <autoglue/TypeDefinition.hh>

namespace ag
{

bool TypeDefinition::matches(const TypeDefinition& other) const
{
	return mType == other.mType;
}

std::shared_ptr <TypeDefinition> TypeDefinition::copyToHeap() const
{
	return std::make_shared <TypeDefinition> (mType);
}

bool TypeDefinition::matchTypeName(std::string_view name) const
{
	// Match type definitions that don't have a special implementation.
	switch (mType)
	{
		case Type::Character: return name == "char";
		case Type::String: return name == "string";
		case Type::Float: return name == "float";
		case Type::Void: return name == "void";

		default: return false;
	}
}

}
