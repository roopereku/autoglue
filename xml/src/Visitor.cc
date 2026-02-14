#include <autoglue/xml/Visitor.hh>
#include <autoglue/xml/ClassImpl.hh>
#include <autoglue/xml/NodeInfo.hh>

#include <string_view>

namespace ag::xml
{

bool Visitor::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute*)
{
	NodeInfo info(element);
	std::string_view tag(element.Name());

	auto type = info.getType();
	if (!type)
	{
		// TODO: Indicate that there was no valid type.
		return false;
	}

	if (!info.getName())
	{
		// TODO: Indicate that there was no name.
		return false;
	}

	switch(*type)
	{
		case Node::Type::Class:
		{
			break;
		}

		case Node::Type::Enum:
		{
			break;
		}

		case Node::Type::EnumValue:
		{
			break;
		}

		case Node::Type::Field:
		{
			break;
		}

		case Node::Type::Function:
		{
			break;
		}

		case Node::Type::Parameter:
		{
			break;
		}

		case Node::Type::Scope:
		{
			break;
		}
	}

	return true;
}

}
