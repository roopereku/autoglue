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
	/// \return True if succesful.
	void generateBindings();

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
	virtual void generateBaseType(TypeEntity& entity, size_t index);

	/// Generates a named scope.
	///
	/// \param entity The ScopeEntity to generate. Only named scopes are generated.
	virtual void generateNamedScope(ScopeEntity& entity);

	/// Generates an argument separator, such as a comma.
	virtual void generateArgumentSeparator();

	/// Generators a return statement for the given return type.
	/// 
	/// \param entity The return type to generate a return statement for.
	/// \param target The function that the return statement is being generated for.
	/// \return Depending on the implementation, true could be returned when an additional enclosure is generated.
	virtual bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target);

	/// Changes the class depth.
	///
	/// \param amount Positive or a negative number indicating which way the class depth should go.
	void changeClassDepth(int amount);

	virtual std::string_view getObjectHandleName();

protected:
	unsigned getClassDepth();

private:
	Backend& backend;
	unsigned classDepth = 0;
};

}

#endif
