#include <autoglue/xml/Deserializer.hh>
#include <autoglue/ScopeEntity.hh>

namespace ag::xml
{

class Deserializer::XMLElementContext : public Deserializer::ElementContext
{
public:
	XMLElementContext(std::shared_ptr <Entity> parent, tinyxml2::XMLElement* element)
		: ElementContext(parent), element(element)
	{
	}

	std::string_view getEntityName() override
	{
		return element->Attribute("name");
	}

	std::shared_ptr <TypeReferenceEntity> getReturnType() override
	{
		return std::make_shared <TypeReferenceEntity> ("", PrimitiveEntity::getVoid(), false);
	}

	FunctionEntity::Type getFunctionGroupType() override
	{
		// TODO: Get the correct type.
		return FunctionEntity::Type::MemberFunction;
	}

	tinyxml2::XMLElement* element;
};

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

	auto rootEntity = std::make_shared <ScopeEntity> ();
	auto* rootElement = document.RootElement();
	addInner(rootElement, rootEntity);

	//return rootEntity;
	return nullptr;
}

void Deserializer::addInner(tinyxml2::XMLElement* element, std::shared_ptr <ag::Entity> parent)
{
	(void)parent;

	auto* current = element->FirstChildElement();
	while(current)
	{
		XMLElementContext ctx(parent, current);
		auto entity = addEntity(current->Name(), ctx);

		if(entity)
		{
			if(entity->getType() != Entity::Type::Function)
			{
				addInner(current, entity);
			}
		}

		//printf("Child '%s' '%s'\n", current->Name(), current->Attribute("name"));
		//addInner(current
		current = current->NextSiblingElement();
	}
}

}
