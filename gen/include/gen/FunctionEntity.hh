#ifndef GEN_FUNCTION_ENTITY_HH
#define GEN_FUNCTION_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class FunctionEntity : public Entity
{
public:
	FunctionEntity(std::string_view name);

	/// Generates this function entity.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator) override;

	const char* getTypeString() override;
};

}

#endif
