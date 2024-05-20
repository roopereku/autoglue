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

	/// Adds a new overload to this function group.
	///
	/// \param overload The overload to add.
	void addOverload(std::shared_ptr <FunctionEntity>&& overload);

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

	/// Creates a new function group containing overrides for interface
	/// functions found in this function group. This makes the interface
	/// functions implicitly used.
	///
	/// \return A new function group containing the overrides or null if there are no interfaces.
	std::shared_ptr <FunctionGroupEntity> createInterfaceOverrides();

private:
	/// Checks if this function group has the given name or
	/// if it matches the given alias name.
	bool hasName(std::string_view str) override;

	/// Generates function overloads.
	void onGenerate(BindingGenerator& generator) override;

	/// Calls use() on every overload of this group.
	void onFirstUse() override;

	FunctionEntity::Type type;
	size_t interfaces = 0;
};

}

#endif
