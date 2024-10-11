#include <autoglue/Serializer.hh>

namespace ag::xml
{

class Serializer : public ag::Serializer
{
public:
	Serializer(Backend& backend);

protected:
	void beginElement(std::string_view name) override;
	void endElement(std::string_view name) override;
};

}
