#ifndef GEN_ENUM_ENTITY_HH
#define GEN_ENUM_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class EnumEntity : public Entity
{
public:
	EnumEntity(std::string_view name);

	/// Generates an enum entity body and the enum values. 
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator) override;

	const char* getTypeString() override;
};

}

#endif
