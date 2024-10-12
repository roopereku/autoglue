#include <autoglue/Serializer.hh>

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
	endElement(entity);	
}

void Serializer::generateEnum(EnumEntity& entity)
{
}

void Serializer::generateEnumEntry(EnumEntryEntity& entity)
{
}

void Serializer::generateFunction(FunctionEntity& entity)
{
}

void Serializer::generateTypeReference(TypeReferenceEntity& entity)
{
}

void Serializer::generateTypeAlias(TypeAliasEntity& entity)
{
}

bool Serializer::generateBaseType(TypeEntity& entity, size_t index)
{
}

void Serializer::generateNamedScope(ScopeEntity& entity)
{
}

void Serializer::generateArgumentSeparator()
{
}

bool Serializer::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
}

}
