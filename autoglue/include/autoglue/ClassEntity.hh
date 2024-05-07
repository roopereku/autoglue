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

	/// Adds a new base class for this class entity.
	///
	/// \param base The base class to add.
	void addBaseClass(std::shared_ptr <ClassEntity> base);

	/// Generates the base classes of this class.
	/// Calls generateTypeReference internally.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateBaseClasses(BindingGenerator& generator);

	/// Checks if this class base classes has any base classes.
	/// Any base class ClassEntity that doesn't exist upon invocation isn't counted.
	///
	/// \return True If this class has base classes.
	bool hasBaseClasses();

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
	std::vector <std::weak_ptr <ClassEntity>> baseClasses;
};

}

#endif
