#ifndef AUTOGLUE_FUNCTION_ENTITY_HH
#define AUTOGLUE_FUNCTION_ENTITY_HH

#include <autoglue/Entity.hh>

namespace ag
{

class TypeReferenceEntity;
class FunctionGroupEntity;

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

	FunctionEntity(std::string_view name, std::string&& postfix, Type type,
			std::shared_ptr <TypeReferenceEntity>&& returnType,
			bool overridable, bool interface);

	/// Gets the parent that's not a function group.
	///
	/// \return The non function group parent entity.
	Entity& getParent() override;

	/// Gets the function group that this function entity is in.
	///
	/// \return The function group that this function entity is in.
	FunctionGroupEntity& getGroup();

	/// Gets the hierarchy leading up to this function.
	/// This override exists so that the function name isn't duplicated which
	/// happens because of the function group parent.
	std::string getHierarchy(const std::string& delimiter = "_") override;

	/// Generates the return type entity of this function.
	/// This isn't called by FunctionEntity::generate as different
	/// languages might want to handle it differently.
	///
	/// \param generator The BindingGenerator to call functions from.
	/// \param asPOD If true, the POD version of the return type is generated.
	void generateReturnType(BindingGenerator& generator, bool asPOD);

	/// Generates the parameters type entities of this function.
	/// This isn't called by FunctionEntity::generate as different
	/// languages might want to handle parameters differently.
	///
	/// \param generator The BindingGenerator to call functions from.
	/// \param asPOD If true, the POD versions of the parameters are generated.
	/// \param includeSelf If true, the first parameter is "self" if it should be present.
	void generateParameters(BindingGenerator& generator, bool asPOD, bool includeSelf);

	/// Gets the parameter count.
	///
	/// \param includeSelf If true, the "self" parameter is counted.
	/// \return The count of parameters.
	size_t getParameterCount(bool includeSelf = false);

	/// Gets the parameter at the given index.
	///
	/// \return The parameter at the given index.
	TypeReferenceEntity& getParameter(size_t index);

	/// Gets the function type.
	///
	/// \return The function type.
	Type getType();

	/// Checks if this function needs a handle to "this".
	/// "this" is the object returned by a constructor glue function.
	///
	/// \return True if "this" handle is needed.
	bool needsThisHandle();

	/// Checks if this function returns a value. A value is returned when
	/// the function is a constructor or has a non-void return type.
	///
	/// \return True if this function returns a value.
	bool returnsValue();

	/// Checks whether this function can be overridden. If a function
	/// cannot be overridden, it should be final.
	///
	/// \return True if this function can be overridden.
	bool isOverridable();

	/// Checks whether this function is an interface.
	///
	/// \return True if this function is an interface.
	bool isInterface();

	/// Gets the return type of this function.
	///
	/// \return The return type of this function.
	TypeReferenceEntity& getReturnType();

	/// Gets the name of the corresponding bridge function.
	///
	/// \return The name of the corresponding bridge function.
	std::string getBridgeName();

	const char* getTypeString() override;

private:
	/// Generates this function entity.
	void onGenerate(BindingGenerator& generator) override;

	/// Makes sure that the return type is used.
	void onFirstUse() override;

	static void generatePOD(BindingGenerator& generator, TypeReferenceEntity ref);

	bool overridable = false;
	bool interface = false;

	std::shared_ptr <TypeReferenceEntity> returnType;
	std::string postfix;
	Type type;
};

}

#endif
