#ifndef AUTOGLUE_XML_TYPE_USAGE_IMPL
#define AUTOGLUE_XML_TYPE_USAGE_IMPL

#include <autoglue/Node.hh>
#include <autoglue/TypeUsage.hh>

#include <tinyxml2.h>

namespace ag::xml
{

class TypeUsageImpl : public AbstractTypeUsage
{
public:
	TypeUsageImpl(tinyxml2::XMLElement& element);

	const AbstractClass& getClass() const override;
	const AbstractEnum& getEnum() const override;
	const Integer getIntegerDefinition() const override;

private:
	static TypeDefinition::Type getMatchingType(tinyxml2::XMLElement& element);
	tinyxml2::XMLElement* findDefinition() const;

	tinyxml2::XMLElement& mElement;
};

}

#endif
