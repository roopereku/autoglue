#include <autoglue/Serializer.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/ScopeEntity.hh>
#include <autoglue/EnumEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>

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
	entity.generateBaseTypes(*this);
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

void Serializer::generateFunctionGroup(FunctionGroupEntity& entity)
{
	beginNestingElement(entity);
	entity.generateOverloads(*this);
	endNestingElement(entity);
}

void Serializer::generateFunction(FunctionEntity& entity)
{
	auto ret = entity.getReturnType();

	beginNestingElement(entity);
	setReturnType(ret);
	entity.generateParameters(*this, false, false);
	endNestingElement(entity);
}

void Serializer::generateTypeReference(TypeReferenceEntity& entity)
{
	beginElement(entity);
	setReferredType(entity.getReferred());
	endElement(entity);
}

void Serializer::generateTypeAlias(TypeAliasEntity& entity)
{
	beginElement(entity);
	setReferredType(*entity.getUnderlying());
	endElement(entity);
}

bool Serializer::generateBaseType(TypeEntity& entity, size_t)
{
	addBaseType(entity);
	return false;
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

bool Serializer::generateReturnStatement(TypeReferenceEntity&, FunctionEntity&)
{
	return false;
}

}
