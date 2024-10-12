#ifndef AUTOGLUE_BINDING_GENERATOR_HH
#define AUTOGLUE_BINDING_GENERATOR_HH

#include <autoglue/Backend.hh>

#include <string_view>

namespace ag
{

class ClassEntity;
class TypeEntity;
class EnumEntity;
class EnumEntryEntity;
class FunctionEntity;
class ScopeEntity;
class TypeReferenceEntity;
class TypeAliasEntity;

class BindingGenerator
{
public:
	/// BindingGenerator constructor.
	///
	/// \param backend The backend to get the root entity from.
	BindingGenerator(Backend& backend);

	/// Generates the bindings.
	///
	/// \param resetEntityContext If true, the previous context of each entity is reset before generation.
	void generateBindings(bool resetEntityContext = true);

	/// Generates a class entity.
	///
	/// \param entity The ClassEntity to generate.
	virtual void generateClass(ClassEntity& entity);

	/// Generates an enum entity.
	///
	/// \param entity The EnumEntity to generate.
	virtual void generateEnum(EnumEntity& entity);

	/// Generates an entry for an enum entity.
	///
	/// \param entity The EnumEntryEntity to generate.
	virtual void generateEnumEntry(EnumEntryEntity& entity);

	/// Generates a function entity.
	///
	/// \param entity The FunctionEntity to generate.
	virtual void generateFunction(FunctionEntity& entity);

	/// Generates a type reference entity.
	///
	/// \param entity The TypeReferenceEntity to generate.
	virtual void generateTypeReference(TypeReferenceEntity& entity);

	/// Generates a type alias entity.
	///
	/// \param entity The TypeAliasEntity to generate.
	virtual void generateTypeAlias(TypeAliasEntity& entity);

	/// Generates a base type specification.
	///
	/// \param entity The TypeEntity containing the base type.
	/// \param index The index of the base type.
	/// \return True if an argument separator may come after the base type.
	virtual bool generateBaseType(TypeEntity& entity, size_t index);

	/// Generates a named scope.
	///
	/// \param entity The ScopeEntity to generate. Only named scopes are generated.
	virtual void generateNamedScope(ScopeEntity& entity);

	/// Generates an argument separator, such as a comma.
	virtual void generateArgumentSeparator();

	/// Generates a return statement for the given return type.
	/// 
	/// \param entity The return type to generate a return statement for.
	/// \param target The function that the return statement is being generated for.
	/// \return Depending on the implementation, true could be returned when an additional enclosure is generated.
	virtual bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target);

	/// Generates a call to the bridge function corresponding to the given function.
	///
	/// \param entity The function to generate the bridge function call for.
	virtual void generateBridgeCall(FunctionEntity& entity);

	/// Generates an interception function.
	///
	/// \param entity The function to generate an interception function for.
	/// \param parentClass The class containing the original function.
	virtual void generateInterceptionFunction(FunctionEntity& entity, ClassEntity& parentClass);

	/// Generates the interception context. This generally should generate a function
	/// that calls the initialization bridge function for interception functions.export
	///
	/// \param entity The class to generate the interception context for.
	virtual void generateInterceptionContext(ClassEntity& entity);

	/// Changes the class depth.
	///
	/// \param amount Positive or a negative number indicating which way the class depth should go.
	void changeClassDepth(int amount);

	virtual std::string_view getObjectHandleName();

	/// Initializes a generator specific generation context for the given entity.
	///
	/// \param entity The entity to initialize a generation context for.
	virtual void initializeGenerationContext(Entity& entity);

protected:
	virtual void onGenerationFinished();

	unsigned getClassDepth();

private:
	Backend& backend;
	unsigned classDepth = 0;
};

}

#endif
