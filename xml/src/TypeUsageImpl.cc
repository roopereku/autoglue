#include <autoglue/xml/TypeUsageImpl.hh>
#include <autoglue/xml/ClassImpl.hh>
#include <autoglue/Token.hh>

#include <cassert>

namespace ag::xml
{

TypeUsageImpl::TypeUsageImpl(tinyxml2::XMLElement& element)
	: AbstractTypeUsage(getMatchingType(element)), mElement(element)
{
}

const AbstractClass& TypeUsageImpl::getClass() const
{
	auto* definition = findDefinition();
	return definition ? std::make_shared <ClassImpl> (*definition) : nullptr;
}

const AbstractEnum& TypeUsageImpl::getEnum() const
{
	auto* definition = findDefinition();
	return definition ? std::make_shared <EnumImpl> (*definition) : nullptr;
}

std::optional <Integer> TypeUsageImpl::getIntegerDefinition() const
{
}

TypeDefinition::Type TypeUsageImpl::getMatchingType(tinyxml2::XMLElement& element)
{
	std::string_view usedType(element.Name());

	if (usedType == "Class")
	{
		return TypeDefinition::Type::Class;
	}

	if (usedType == "Enum")
	{
		return TypeDefinition::Type::Enum;
	}

	if (usedType == "Integer")
	{
		return TypeDefinition::Type::Integer;
	}

	// TODO: Match the rest.
	assert(false);
}

tinyxml2::XMLElement* TypeUsageImpl::findDefinition() const
{
	auto definition = mElement.FirstChildElement("Definition");
	if (!definition)
	{
		return nullptr;
	}

	auto current = mElement.GetDocument()->RootElement();

	bool foundDelimiter;
	std::string_view path(definition->GetText());
	auto token = extractUntilOrNextToken(path, '.', foundDelimiter);

	while (current)
	{
		// Does the name of the current element match the current token?
		auto nameElement = current->FirstChildElement("Name");
		if (nameElement && token == nameElement->Value())
		{
			if (!foundDelimiter)
			{
				return current;
			}

			// Move on to an inner element and match against the next token.
			token = extractUntilOrNextToken(path, '.', foundDelimiter);
			current = current->FirstChildElement();
		}

		else
		{
			// No match here. check the next sibling.
			current = current->NextSiblingElement();
		}
	}

	return current;
}

}
