#include <autoglue/xml/NodeInfo.hh>

namespace ag::xml
{

const tinyxml2::XMLElement& NodeInfo::getElement() const
{
	return mElement;
}

std::optional <std::string_view> NodeInfo::getName() const
{
	if (auto nameElement = mElement.FirstChildElement("Name"))
	{
		return nameElement->GetText();
	}

	return {};
}

std::optional <Node::Type> NodeInfo::getType() const
{
	std::string_view tag(mElement.Name());

	if (tag == "Class")
	{
		return Node::Type::Class;
	}

	return {};
}

}
