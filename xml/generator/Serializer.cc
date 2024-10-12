#include <autoglue/xml/Serializer.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <tinyxml2.h>

#include <cassert>

namespace ag::xml
{

Serializer::Serializer(Backend& backend)
	: ag::Serializer(backend)
{
	auto* root = document.NewElement("hierarchy");
	document.InsertFirstChild(root);
	trail.push(root);
}

void Serializer::writeElement(Entity& entity)
{
	assert(!trail.empty());

	current = document.NewElement(entity.getTypeString());
	current->SetAttribute("name", entity.getName().c_str());
	trail.top()->InsertEndChild(current);
}

void Serializer::beginElement(Entity& entity)
{
	writeElement(entity);
	trail.push(current);
}

void Serializer::endElement(Entity& entity)
{
	trail.pop();
}

void Serializer::setReturnValue(FunctionEntity& entity)
{
}

void Serializer::setReferredType(TypeAliasEntity& entity)
{
	current->SetAttribute("referred", entity.getUnderlying()->getHierarchy(".").c_str());
}

void Serializer::addBaseType(TypeEntity& entity)
{
}

void Serializer::onGenerationFinished()
{
	document.SaveFile("hierarchy.xml");
	document.Clear();
}

}
