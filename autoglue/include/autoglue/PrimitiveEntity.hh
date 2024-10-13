#ifndef AUTOGLUE_PRIMITIVE_ENTITY_HH
#define AUTOGLUE_PRIMITIVE_ENTITY_HH

#include <autoglue/TypeEntity.hh>

namespace ag
{

class PrimitiveEntity : public TypeEntity
{
public:
	enum class Type
	{
		ObjectHandle,
		Integer,
		Float,
		Double,
		Boolean,
		Character,
		String,
		Void
	};

	static std::shared_ptr <PrimitiveEntity> getObjectHandle();
	static std::shared_ptr <PrimitiveEntity> getInteger();
	static std::shared_ptr <PrimitiveEntity> getFloat();
	static std::shared_ptr <PrimitiveEntity> getDouble();
	static std::shared_ptr <PrimitiveEntity> getBoolean();
	static std::shared_ptr <PrimitiveEntity> getCharacter();
	static std::shared_ptr <PrimitiveEntity> getString();
	static std::shared_ptr <PrimitiveEntity> getVoid();

	Type getType();

	const char* getTypeString() override;

private:
	PrimitiveEntity(std::string_view name, Type type);

	void onGenerate(BindingGenerator&) override
	{
		// TODO: Call something like generatePrimitive if necessary.
	}

	Type type;
};

}

#endif
