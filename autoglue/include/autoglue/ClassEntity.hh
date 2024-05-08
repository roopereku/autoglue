#ifndef AUTOGLUE_CLASS_ENTITY_HH
#define AUTOGLUE_CLASS_ENTITY_HH

#include <autoglue/TypeEntity.hh>

namespace ag
{

class TypeReferenceEntity;

class ClassEntity : public TypeEntity
{
public:
	ClassEntity(std::string_view name);

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
