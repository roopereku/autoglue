#ifndef GEN_BINDING_GENERATOR_HH
#define GEN_BINDING_GENERATOR_HH

#include <gen/Backend.hh>

namespace gen
{

class ClassEntity;
class EnumEntity;
class EnumEntryEntity;
class FunctionEntity;
class ScopeEntity;
class TypeReferenceEntity;

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

	/// Generates the beginning of a class entity.
	///
	/// \param entity The ClassEntity to generate the beginning of.
	virtual void generateClassBeginning(ClassEntity& entity) = 0;

	/// Generates the ending of a class entity.
	///
	/// \param entity The ClassEntity to generate the ending of.
	virtual void generateClassEnding(ClassEntity& entity) = 0;

	/// Generates the beginning of an enum entity.
	///
	/// \param entity The EnumEntity to generate the beginning of.
	virtual void generateEnumBeginning(EnumEntity& entity) = 0;

	/// Generates the ending of an enum entity.
	///
	/// \param entity The EnumEntity to generate the ending of.
	virtual void generateEnumEnding(EnumEntity& entity) = 0;

	/// Generates an entry for an enum entity.
	///
	/// \param entity The EnumEntryEntity to generate.
	virtual void generateEnumEntry(EnumEntryEntity& entity) = 0;

	/// Generates a function entity.
	///
	/// \param entity The FunctionEntity to generate.
	virtual void generateFunction(FunctionEntity& entity) = 0;

	/// Generates a type reference entity.
	///
	/// \param entity The TypeReferenceEntity to generate.
	virtual void generateTypeReference(TypeReferenceEntity& entity) = 0;

	/// Generates a base class specification.
	///
	/// \param entity The ClassEntity containing the base class.
	/// \param index The index of the base class.
	virtual void generateBaseClass(ClassEntity& entity, size_t index) = 0;

	/// Generates the beginning of a named scope.
	///
	/// \param entity The ScopeEntity to generate a beginning for. This should always have a name.
	virtual void generateNamedScopeBeginning(ScopeEntity& entity) = 0;

	/// Generates the ending of a named scope.
	///
	/// \param entity The ScopeEntity to generate a ending for. This should always have a name.
	virtual void generateNamedScopeEnding(ScopeEntity& entity) = 0;

	/// Generates an argument separator, such as a comma.
	virtual void generateArgumentSeparator() = 0;

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
