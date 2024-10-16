#ifndef AUTOGLUE_XML_DESERIALIZER_HH
#define AUTOGLUE_XML_DESERIALIZER_HH

#include <autoglue/Deserializer.hh>

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
	std::string path;
};

}

#endif
