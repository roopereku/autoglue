#ifndef GEN_FUNCTION_ENTITY_HH
#define GEN_FUNCTION_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class FunctionEntity : public Entity
{
public:
	FunctionEntity(std::string_view name);

	/// Generates the return type entity of this function.
	/// This isn't called by FunctionEntity::generate as different
	/// languages might want to handle it differently.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateReturnType(BindingGenerator& generator);

	/// Generates the parameters type entities of this function.
	/// This isn't called by FunctionEntity::generate as different
	/// languages might want to handle parameters differently.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateParameters(BindingGenerator& generator);

	/// Gets the parameter count.
	///
	/// \return The count of parameters.
	size_t getParameterCount();

	const char* getTypeString() override;

private:
	/// Generates this function entity.
	void onGenerate(BindingGenerator& generator) override;
};

}

#endif
