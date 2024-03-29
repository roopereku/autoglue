#ifndef AUTOGLUE_BINDING_GENERATOR_HH
#define AUTOGLUE_BINDING_GENERATOR_HH

#include <autoglue/Backend.hh>

namespace ag
{

class ClassEntity;
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

	/// Generates a base class specification.
	///
	/// \param entity The ClassEntity containing the base class.
	/// \param index The index of the base class.
	virtual void generateBaseClass(ClassEntity& entity, size_t index);

	/// Generates a named scope.
	///
	/// \param entity The ScopeEntity to generate. Only named scopes are generated.
	virtual void generateNamedScope(ScopeEntity& entity);

	/// Generates an argument separator, such as a comma.
	virtual void generateArgumentSeparator();

	/// Changes the class depth.
	///
	/// \param amount Positive or a negative number indicating which way the class depth should go.
	void changeClassDepth(int amount);

protected:
	unsigned getClassDepth();

private:
	Backend& backend;
	unsigned classDepth = 0;
};

}

#endif
