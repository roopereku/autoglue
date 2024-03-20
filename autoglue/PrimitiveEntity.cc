#include <autoglue/PrimitiveEntity.hh>

#include <cassert>

namespace ag
{

PrimitiveEntity::PrimitiveEntity(std::string_view name, Type type)
	: TypeEntity(name, TypeEntity::Type::Primitive), type(type)
{
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getObjectHandle()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("ObjectHandle", Type::ObjectHandle));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getInteger()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("Integer", Type::Integer));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getFloat()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("Float", Type::Float));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getDouble()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("Double", Type::Double));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getBoolean()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("Boolean", Type::Boolean));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getCharacter()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("Character", Type::Character));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getString()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("String", Type::String));
	return entity;
}

std::shared_ptr <PrimitiveEntity> PrimitiveEntity::getVoid()
{
	static std::shared_ptr <PrimitiveEntity> entity(new PrimitiveEntity("Void", Type::Void));
	return entity;
}

PrimitiveEntity::Type PrimitiveEntity::getType()
{
	return type;
}

const char* PrimitiveEntity::getTypeString()
{
	return "Primitive type";
}

}
