#include <autoglue/xml/Serializer.hh>
#include <tinyxml2.h>

namespace ag::xml
{

Serializer::Serializer(Backend& backend)
	: ag::Serializer(backend)
{
}

void Serializer::beginElement(Entity& entity)
{
}

void Serializer::endElement(Entity& entity)
{
}

}
