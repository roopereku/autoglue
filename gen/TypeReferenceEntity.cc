#include <gen/TypeReferenceEntity.hh>
#include <gen/BindingGenerator.hh>

#include <iostream>
#include <cassert>

namespace gen
{

TypeReferenceEntity::TypeReferenceEntity(std::string_view name, std::shared_ptr <ClassEntity> classType)
	: Entity(name), referred(classType), type(Type::Class)
{
}

TypeReferenceEntity::TypeReferenceEntity(std::string_view name, std::shared_ptr <EnumEntity> classType)
	: Entity(name), referred(classType), type(Type::Enum)
{
}

void TypeReferenceEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateTypeReference(*this);
}

TypeReferenceEntity::Type TypeReferenceEntity::getType()
{
	return type;
}

Entity& TypeReferenceEntity::getReferred()
{
	return *referred;
}

bool TypeReferenceEntity::isClass()
{
	return type == Type::Class;
}

ClassEntity& TypeReferenceEntity::getClassType()
{
	assert(type == Type::Class);
	return static_cast <ClassEntity&> (*referred);
}

bool TypeReferenceEntity::isEnum()
{
	return type == Type::Enum;
}

EnumEntity& TypeReferenceEntity::getEnumType()
{
	assert(type == Type::Enum);
	return static_cast <EnumEntity&> (*referred);
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
