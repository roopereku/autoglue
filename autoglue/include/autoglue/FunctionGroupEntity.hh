#ifndef AUTOGLUE_FUNCTION_GROUP_ENTITY_HH
#define AUTOGLUE_FUNCTION_GROUP_ENTITY_HH

#include <autoglue/FunctionEntity.hh>

namespace ag
{

// Represents a collection of function overloads.
class FunctionGroupEntity : public Entity
{
public:
	FunctionGroupEntity(std::string_view name, FunctionEntity::Type type);

	const char* getTypeString() override;

	/// Find a function from this function group that has matching
	/// parameter types to the given function.
	///
	/// \param entity The function to match against.
	/// \return Function entity containing matching parameters if any.
	std::shared_ptr <FunctionEntity> findMatchingParameters(FunctionEntity& entity);

	/// Gets the amount of overloads within this function group.
	///
	/// \return The amount of overlaods within this function group.
	size_t getOverloadCount();

	/// Gets the nth function overload within this function group.
	///
	/// \return The nth function overload within this function group.
	FunctionEntity& getOverload(size_t index);

	/// Gets the type of the functions that this FunctionGroupEntity contains.
	///
	/// \return The type of the contained functions.
	FunctionEntity::Type getType();

private:
	/// Checks if this function group has the given name or
	/// if it matches the given alias name.
	bool hasName(std::string_view str) override;

	/// Generates function overloads.
	void onGenerate(BindingGenerator& generator) override;

	/// Ignores functions which already exist.
	bool interceptNewChild(Entity& newChild) override;

	FunctionEntity::Type type;
};

}

#endif
