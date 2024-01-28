#ifndef GEN_ENUM_ENTRY_ENTITY_HH
#define GEN_ENUM_ENTRY_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class EnumEntryEntity : public Entity
{
public:
	EnumEntryEntity(std::string_view name);

	/// Generates this enum entry.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator) override;

	const char* getTypeString() override;
};

}

#endif
