#include <gen/TypeReferenceEntity.hh>
#include <gen/BindingGenerator.hh>

#include <iostream>
#include <cassert>

namespace gen
{

TypeReferenceEntity::TypeReferenceEntity(std::string_view name, std::shared_ptr <TypeEntity> type)
	: Entity(name), referred(type)
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
	return *referred;
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

bool TypeReferenceEntity::isPrimitive()
{
	return getType() == TypeEntity::Type::Primitive;
}

PrimitiveEntity& TypeReferenceEntity::getPrimitiveType()
{
	assert(isPrimitive());
	return static_cast <PrimitiveEntity&> (*referred);
}

void TypeReferenceEntity::setContext(std::shared_ptr <TypeContext>&& context)
{
	this->context = std::move(context);
}

std::shared_ptr <TypeContext> TypeReferenceEntity::getContext()
{
	return context;
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
