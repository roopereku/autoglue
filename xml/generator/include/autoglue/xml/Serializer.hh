#include <autoglue/Serializer.hh>
#include <tinyxml2.h>

#include <stack>

namespace ag::xml
{

class Serializer : public ag::Serializer
{
public:
	Serializer(Backend& backend);

protected:
	void beginElement(Entity& entity) override;
	void endElement(Entity& entity) override;

	void setReturnValue(FunctionEntity& entity) override;
	void setReferredType(TypeAliasEntity& entity) override;
	void addBaseType(TypeEntity& entity) override;
	void endNestingElement(Entity& entity) override;

	void onGenerationFinished() override;

private:
	tinyxml2::XMLDocument document;
	tinyxml2::XMLElement* current = nullptr;
	std::stack <tinyxml2::XMLElement*> trail;
};

}
