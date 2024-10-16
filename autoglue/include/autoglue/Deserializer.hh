#ifndef AUTOGLUE_DESERIALIZER_HH
#define AUTOGLUE_DESERIALIZER_HH

#include <autoglue/Entity.hh>
#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/FunctionEntity.hh>

namespace ag
{

class Deserializer
{
public:
	virtual ~Deserializer();

	virtual std::shared_ptr <Entity> createHierarchy() = 0;

protected:
	class ElementContext;

	Deserializer();

	std::shared_ptr <Entity> addEntity(std::string_view type, ElementContext& ctx); 
};

class Deserializer::ElementContext
{
public:
	virtual ~ElementContext() {}

	virtual std::string_view getEntityName() = 0;
	virtual std::shared_ptr <TypeReferenceEntity> getReturnType() = 0;
	virtual FunctionEntity::Type getFunctionGroupType() = 0;

	std::shared_ptr <Entity> getParent()
	{
		return parent;
	}

protected:
	ElementContext(std::shared_ptr <Entity> parent)
		: parent(parent)
	{
	}	

private:
	std::shared_ptr <Entity> parent;
};

}

#endif
