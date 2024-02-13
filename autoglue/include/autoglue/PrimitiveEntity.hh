#ifndef AUTOGLUE_PRIMITIVE_ENTITY_HH
#define AUTOGLUE_PRIMITIVE_ENTITY_HH

#include <autoglue/TypeEntity.hh>

namespace ag
{

class PrimitiveEntity : public TypeEntity
{
public:
	PrimitiveEntity(std::string_view name)
		: TypeEntity(name, Type::Primitive)
	{
	}

	const char* getTypeString() override
	{
		return "Primitive";
	}

private:
	void onGenerate(BindingGenerator& generator) override
	{
		// TODO: Call something like generatePrimitive if necessary.
	}
};

}
#endif
