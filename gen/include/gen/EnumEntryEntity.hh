#ifndef GEN_ENUM_ENTRY_ENTITY_HH
#define GEN_ENUM_ENTRY_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class EnumEntryEntity : public Entity
{
public:
	EnumEntryEntity(std::string_view name);

	const char* getTypeString() override;

private:
	/// Generates an enum entry.
	void onGenerate(BindingGenerator& generator) override;

};

}

#endif
