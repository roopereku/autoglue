#ifndef GEN_ENUM_ENTITY_HH
#define GEN_ENUM_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class EnumEntity : public Entity
{
public:
	EnumEntity(std::string_view name);

	const char* getTypeString() override;

private:
	/// Generates enum values wrapped within an enum beginning and ending.
	void onGenerate(BindingGenerator& generator) override;

};

}

#endif
