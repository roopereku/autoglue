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

void Serializer::generateClass(ClassEntity& entity)
{
	beginElement(entity);
	entity.generateNested(*this);
	endElement(entity);	
}

void Serializer::generateEnum(EnumEntity& entity)
{
	beginElement(entity);
	entity.generateValues(*this);
	endElement(entity);	
}

void Serializer::generateEnumEntry(EnumEntryEntity& entity)
{
	writeElement(entity);
}

void Serializer::generateFunction(FunctionEntity& entity)
{
}

void Serializer::generateTypeReference(TypeReferenceEntity& entity)
{
}

void Serializer::generateTypeAlias(TypeAliasEntity& entity)
{
	writeElement(entity);
	setReferredType(entity);
}

bool Serializer::generateBaseType(TypeEntity& entity, size_t index)
{
}

void Serializer::generateNamedScope(ScopeEntity& entity)
{
	beginElement(entity);
	entity.generateNested(*this);
	endElement(entity);	
}

void Serializer::generateArgumentSeparator()
{
}

bool Serializer::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
}

}
