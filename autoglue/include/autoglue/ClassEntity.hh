#ifndef AUTOGLUE_CLASS_ENTITY_HH
#define AUTOGLUE_CLASS_ENTITY_HH

#include <autoglue/TypeEntity.hh>

#include <memory>

namespace ag
{

class TypeReferenceEntity;

class ClassEntity : public TypeEntity,
					public std::enable_shared_from_this <ClassEntity>
{
public:
	ClassEntity(std::string_view name);

	/// Adds a nested entity for this class.
	///
	/// \param nested The entity to add to this class.
	void addNested(std::shared_ptr <Entity>&& nested);

	/// Adds a new base type for this class entity.
	///
	/// \param base The base type to add.
	void addBaseType(std::shared_ptr <TypeEntity> base);

	/// Generates the base types of this class.
	/// Calls generateTypeReference internally.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateBaseTypes(BindingGenerator& generator);

	/// Checks if this class has any existing base types.
	///
	/// \return True If this class has base classes.
	bool hasBaseTypes();

	/// Gets the amount of base types for this class.
	///
	/// \return The amount of base types for this class.
	size_t getBaseTypeCount();

	/// Gets the nth base type for this class.
	///
	/// \param The index of the desired base type.
	/// \return The base type at the given index.
	TypeEntity& getBaseType(size_t index);

	/// Gets the amount of classes that derive from this class.
	///
	/// \return The amount of classes that derive from this class.
	size_t getDerivedCount();

	/// Gets the nth derived class.
	///
	/// \param The index of the desired derived class.
	/// \return The derived class at the given index.
	ClassEntity& getDerived(size_t index);

	/// Generates the nested entities within this class.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateNested(BindingGenerator& generator);

	/// Checks whether this class is abstract.
	///
	/// \return True if this class is abstract.
	bool isAbstract();

	/// Generates a concrete type for this class if it's present.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateConcreteType(BindingGenerator& generator);

	/// Gets the concrete type of this class.
	///
	/// \return The concrete type of this class or nullptr.
	std::shared_ptr <ClassEntity> getConcreteType();

	/// Checks whether this class entity is a concrete type for
	/// another class entity.
	///
	/// \return True if this is a concrete type.
	bool isConcreteType();

	/// Generates the interception functions of this class.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateInterceptionFunctions(BindingGenerator& generator);

	/// Generates an interception context for this class.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateInterceptionContext(BindingGenerator& generator);

	/// Checks whether the given entity is a ClassEntity.
	///
	/// \param entity The entity to check.
	/// \return True if the given entity is a ClassEntity.
	static bool matchType(Entity& entity);

	/// Finds the first occurence of an overridable FunctionEntity of the given name
	/// and signature from any base class of this class. This is done recurrsively.
	///
	/// \param entity The FunctionEntity that specified the name and signature
	/// \return The matching overridable function or nullptr.
	std::shared_ptr <FunctionEntity> findOverridableFromBase(FunctionEntity& entity);

	const char* getTypeString() override;

private:
	/// Generates the child entities wrapped inside class beginning and ending.
	void onGenerate(BindingGenerator& generator) override;

	/// Makes sure that the base classes and constructors are used.
	void onFirstUse() override;

	/// Checks class abstractness and initializes the concrete type if necessary.
	void onInitialize() override;

	/// Adds interface overrides of this class to the given concrete type.
	void addOverridesToConcrete(std::shared_ptr <ClassEntity> concrete);

	bool abstract = false;
	bool isConcrete = false;

	std::vector <std::weak_ptr <TypeEntity>> baseTypes;
	std::vector <std::weak_ptr <ClassEntity>> derivedClasses;

	std::shared_ptr <ClassEntity> concreteType;
};

}

#endif
