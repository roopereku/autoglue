#ifndef AUTOGLUE_TYPE_REFERENCE_ENTITY_HH
#define AUTOGLUE_TYPE_REFERENCE_ENTITY_HH

#include <autoglue/ClassEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/EnumEntity.hh>
#include <autoglue/PrimitiveEntity.hh>

namespace ag
{

class TypeReferenceEntity : public Entity
{
public:
	TypeReferenceEntity(std::string_view name, std::shared_ptr <TypeEntity> type, bool reference);

	/// Checks if this type reference is identical to another one.
	///
	/// \param other The type reference to compare against.
	/// \return True If this and the given type reference are identical.
	bool isIdentical(const TypeReferenceEntity& other) const;

	/// Gets the type of referenced type entity.
	///
	/// \return The type of the referenced type entity.
	TypeEntity::Type getType();

	/// Gets the referred type entity as an entity.
	///
	/// \return The referred type entity as an entity.
	TypeEntity& getReferred();

	/// Gets the referred type entity as a shared pointer.
	///
	/// \return The referred type entity as a shared pointer.
	std::shared_ptr <TypeEntity> getReferredPtr();

	/// Checks if this type reference refers to a class type.
	///
	/// \return True if this type reference refers to a class type.
	bool isClass();

	/// Gets the referred type entity as a class type.
	///
	/// \return The referred type entity as a class type.
	ClassEntity& getClassType();

	/// Checks if this type reference refers to an enum type.
	///
	/// \return True if this type reference refers to an enum type.
	bool isEnum();

	/// Gets the referred type entity as an enum type.
	///
	/// \return The referred type entity as an enum type.
	EnumEntity& getEnumType();

	/// Checks if this type reference refers to an alias type.
	///
	/// \return True if this type reference refers to an alias type.
	bool isAlias();

	/// Gets the referred type entity as an alias type.
	///
	/// \return The referred type entity as an alias type.
	TypeAliasEntity& getAliasType();

	/// Checks if this type reference refers to a primitive type.
	///
	/// \return True if this type reference refers to a primitive type.
	bool isPrimitive();

	/// Gets the referred type entity as a primitive type.
	///
	/// \return The referred type entity as a primitive type.
	PrimitiveEntity& getPrimitiveType();

	/// Checks whether this type reference entity represents a reference.
	/// This information is especially important in return values, as it
	/// determines if a foreign language should own a returned object or not.
	///
	/// \return True if this type reference entity represents a reference.
	bool isReference();

	/// Returns the POD representation of this type reference.
	///
	/// \return The POD representation of this type reference.
	TypeReferenceEntity getAsPOD();

	const char* getTypeString() override;

private:
	/// Generates this type reference entity.
	void onGenerate(BindingGenerator& generator) override;

	/// Increment the usage counter of the referred type.
	void onFirstUse() override;

	void onList(std::string_view indent) override;

	std::shared_ptr <TypeEntity> referred;
	bool reference = false;
	bool unsignedType = false;
	size_t size = 0;
};

}

#endif
