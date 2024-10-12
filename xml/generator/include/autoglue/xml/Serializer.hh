#include <autoglue/Serializer.hh>

namespace ag::xml
{

class Serializer : public ag::Serializer
{
public:
	Serializer(Backend& backend);

protected:
	void beginElement(Entity& entity) override;
	void endElement(Entity& entity) override;
};

}
