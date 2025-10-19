#ifndef AUTOGLUE_XML_NODE_INFO_HH
#define AUTOGLUE_XML_NODE_INFO_HH

#include <autoglue/Node.hh>

#include <tinyxml2.h>

#include <string_view>
#include <optional>

namespace ag::xml
{

class NodeInfo
{
public:
	NodeInfo(const tinyxml2::XMLElement& element)
		: mElement(element)
	{
	}

	const tinyxml2::XMLElement& getElement() const;
	std::optional <std::string_view> getName() const;
	std::optional <Node::Type> getType() const;

private:
	const tinyxml2::XMLElement& mElement;
	Node::Type mType;
};

}

#endif
