#ifndef GEN_PRIMITIVE_ENTITY_HH
#define GEN_PRIMITIVE_ENTITY_HH

#include <gen/TypeEntity.hh>

namespace gen
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
