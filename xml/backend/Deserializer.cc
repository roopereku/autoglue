#include <autoglue/xml/Deserializer.hh>

#include <tinyxml2.h>

namespace ag::xml
{

Deserializer::Deserializer(std::string_view path)
	: path(path)
{
}

std::shared_ptr <ag::Entity> Deserializer::createHierarchy()
{
	tinyxml2::XMLDocument document;
	if(document.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
	{
		printf("XML: Unable to open file '%s' for deserialization\n", path.c_str());
		return nullptr;
	}

	auto* root = document.RootElement();
	printf("Root '%s'\n", root->Name());

	return nullptr;
}

}
