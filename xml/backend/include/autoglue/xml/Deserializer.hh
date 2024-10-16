#ifndef AUTOGLUE_XML_DESERIALIZER_HH
#define AUTOGLUE_XML_DESERIALIZER_HH

#include <autoglue/Deserializer.hh>

#include <tinyxml2.h>

#include <string_view>
#include <string>

namespace ag::xml
{

class Deserializer : public ag::Deserializer
{
public:
	Deserializer(std::string_view path);

	std::shared_ptr <ag::Entity> createHierarchy() override;

private:
	class XMLElementContext;

	void addInner(tinyxml2::XMLElement* element, std::shared_ptr <ag::Entity> parent);

	std::string path;
};

}

#endif
