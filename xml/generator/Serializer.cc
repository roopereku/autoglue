#include <autoglue/xml/Serializer.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

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

void Serializer::beginElement(Entity& entity)
{
	assert(!trail.empty());

	current = document.NewElement(entity.getTypeString());
	current->SetAttribute("name", entity.getName().c_str());
	trail.top()->InsertEndChild(current);

	trail.push(current);
}

void Serializer::endElement(Entity&)
{
	trail.pop();
}

void Serializer::setReturnType(TypeReferenceEntity& entity)
{
	auto* element = document.NewElement("Return");

	// TODO: Handle qualifiers?
	element->SetText(entity.getReferred().getHierarchy(".").c_str());
	trail.top()->InsertEndChild(element);
}

void Serializer::setReferredType(TypeEntity& entity)
{
	auto* element = document.NewElement("Referred");
	element->SetText(entity.getHierarchy(".").c_str());
	trail.top()->InsertEndChild(element);
}

void Serializer::addBaseType(TypeEntity& entity)
{
	auto* element = document.NewElement("Base");
	element->SetText(entity.getHierarchy(".").c_str());
	trail.top()->InsertEndChild(element);
}

void Serializer::onGenerationFinished()
{
	document.SaveFile("hierarchy.xml");
	document.Clear();
}

}
