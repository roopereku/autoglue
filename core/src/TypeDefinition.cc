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

bool TypeDefinition::matchTypeName(std::wstring_view name) const
{
	// Match type definitions that don't have a special implementation.
	switch (mType)
	{
		case Type::Character: return name == L"char";
		case Type::String: return name == L"string";
		case Type::Float: return name == L"float";
		case Type::Void: return name == L"void";

		default: return false;
	}
}

}
