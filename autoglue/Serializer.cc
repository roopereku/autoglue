#include <autoglue/Serializer.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/ScopeEntity.hh>
#include <autoglue/EnumEntity.hh>
#include <autoglue/TypeAliasEntity.hh>

namespace ag
{

Serializer::Serializer(Backend& backend)
	: BindingGenerator(backend)
{
}

void Serializer::serialize()
{
	generateBindings();
}

void Serializer::beginNestingElement(Entity& entity)
{
	beginElement(entity);
}

void Serializer::endNestingElement(Entity& entity)
{
	endElement(entity);
}

void Serializer::generateClass(ClassEntity& entity)
{
	beginNestingElement(entity);
	entity.generateNested(*this);
	endNestingElement(entity);	
}

void Serializer::generateEnum(EnumEntity& entity)
{
	beginNestingElement(entity);
	entity.generateValues(*this);
	endNestingElement(entity);	
}

void Serializer::generateEnumEntry(EnumEntryEntity& entity)
{
	beginElement(entity);
	endElement(entity);
}

void Serializer::generateFunction(FunctionEntity& entity)
{
}

void Serializer::generateTypeReference(TypeReferenceEntity& entity)
{
}

void Serializer::generateTypeAlias(TypeAliasEntity& entity)
{
	beginElement(entity);
	setReferredType(entity);
	endElement(entity);
}

bool Serializer::generateBaseType(TypeEntity& entity, size_t index)
{
}

void Serializer::generateNamedScope(ScopeEntity& entity)
{
	beginNestingElement(entity);
	entity.generateNested(*this);
	endNestingElement(entity);	
}

void Serializer::generateArgumentSeparator()
{
}

bool Serializer::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
}

}
