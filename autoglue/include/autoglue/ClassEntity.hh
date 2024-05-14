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

	/// Generates the nested entities within this class.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateNested(BindingGenerator& generator);

	/// Checks whether this class is abstract.
	///
	/// \return True if this class is abstract.
	bool isAbstract();

	/// Sets this class as abstract.
	void setAbstract();

	const char* getTypeString() override;

private:
	/// Generates the child entities wrapped inside class beginning and ending.
	void onGenerate(BindingGenerator& generator) override;

	/// Makes sure that the base classes are used.
	void onFirstUse() override;

	bool abstract = false;
	std::vector <std::weak_ptr <TypeEntity>> baseTypes;
};

}

#endif
