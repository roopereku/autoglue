#include <autoglue/Deserializer.hh>

#include <string_view>

namespace ag::xml
{

class Deserializer : public ag::Deserializer
{
public:
	Deserializer(std::string_view path);
};

}
