#ifndef GEN_FUNCTION_ENTITY_HH
#define GEN_FUNCTION_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class FunctionEntity : public Entity
{
public:
	enum class Type
	{
		Constructor,
		Destructor,
		MemberFunction,
		Function
	};

	FunctionEntity(std::string_view name, Type type);

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

	/// Gets the function type.
	///
	/// \return The function type.
	Type getType();

	/// Checks if this function needs a handle to "this".
	/// "this" is the object returned by a constructor glue function.
	///
	/// \return True if "this" handle is needed.
	bool needsThisHandle();

	const char* getTypeString() override;

private:
	/// Generates this function entity.
	void onGenerate(BindingGenerator& generator) override;

	Type type;
};

}

#endif
