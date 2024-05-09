#ifndef AUTOGLUE_FUNCTION_GROUP_ENTITY_HH
#define AUTOGLUE_FUNCTION_GROUP_ENTITY_HH

#include <autoglue/FunctionEntity.hh>

namespace ag
{

class FunctionGroupEntity : public Entity
{
public:
	FunctionGroupEntity(std::string_view name);

	const char* getTypeString() override;

	/// Find a function from this function group that has matching
	/// parameter types to the given function.
	///
	/// \param entity The function to match against.
	/// \return Function entity containing matching parameters if any.
	std::shared_ptr <FunctionEntity> findMatchingParameters(FunctionEntity& entity);

private:
	/// Generates function overloads.
	void onGenerate(BindingGenerator& generator) override;

	/// Ignores functions which already exist.
	bool interceptNewChild(Entity& newChild) override;
};

}

#endif
