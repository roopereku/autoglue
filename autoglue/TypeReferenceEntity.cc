#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/BindingGenerator.hh>

#include <iostream>
#include <cassert>

namespace ag
{

TypeReferenceEntity::TypeReferenceEntity(std::string_view name, std::shared_ptr <TypeEntity> type, bool reference)
	: Entity(name), referred(type), reference(reference)
{
}

bool TypeReferenceEntity::isIdentical(const TypeReferenceEntity& other) const
{
	return referred == other.referred;
}

void TypeReferenceEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateTypeReference(*this);
}

TypeEntity::Type TypeReferenceEntity::getType()
{
	assert(referred);
	return referred->getType();
}

Entity& TypeReferenceEntity::getReferred()
{
	assert(referred);
	return *referred;
}

std::shared_ptr <Entity> TypeReferenceEntity::getReferredPtr()
{
	return referred;
}

bool TypeReferenceEntity::isClass()
{
	return getType() == TypeEntity::Type::Class;
}

ClassEntity& TypeReferenceEntity::getClassType()
{
	assert(isClass());
	return static_cast <ClassEntity&> (*referred);
}

bool TypeReferenceEntity::isEnum()
{
	return getType() == TypeEntity::Type::Enum;
}

EnumEntity& TypeReferenceEntity::getEnumType()
{
	assert(isEnum());
	return static_cast <EnumEntity&> (*referred);
}

bool TypeReferenceEntity::isAlias()
{
	return getType() == TypeEntity::Type::Alias;
}

TypeAliasEntity& TypeReferenceEntity::getAliasType()
{
	assert(isAlias());
	return static_cast <TypeAliasEntity&> (*referred);
}

bool TypeReferenceEntity::isPrimitive()
{
	return getType() == TypeEntity::Type::Primitive;
}

PrimitiveEntity& TypeReferenceEntity::getPrimitiveType()
{
	assert(isPrimitive());
	return static_cast <PrimitiveEntity&> (*referred);
}

bool TypeReferenceEntity::isReference()
{
	return reference;
}

const char* TypeReferenceEntity::getTypeString()
{
	return "TypeReference";
}

void TypeReferenceEntity::onList(std::string_view indent)
{
	Entity::onList(indent);

	if(referred)
	{
		std::cout << indent << " -> " << referred->getName() << '\n';
	}
}

void TypeReferenceEntity::onFirstUse()
{
	assert(referred);
	referred->use();
}

}
