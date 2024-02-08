#ifndef GEN_TYPE_REFERENCE_ENTITY_HH
#define GEN_TYPE_REFERENCE_ENTITY_HH

#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>
#include <gen/PrimitiveEntity.hh>
#include <gen/TypeContext.hh>

namespace gen
{

class TypeReferenceEntity : public Entity
{
public:
	TypeReferenceEntity(std::string_view name, std::shared_ptr <TypeEntity> type);

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
	Entity& getReferred();

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

	/// Checks if this type reference refers to a primitive type.
	///
	/// \return True if this type reference refers to a primitive type.
	bool isPrimitive();

	/// Gets the referred type entity as a primitive type.
	///
	/// \return The referred type entity as a primitive type.
	PrimitiveEntity& getPrimitiveType();

	/// Sets the context for this type reference. This can be anything that
	/// a backend might require when generating glue code.
	///
	/// \param context Context for this type.
	void setContext(std::shared_ptr <TypeContext>&& context);

	/// Gets the context for this type reference.
	///
	/// \return The context for this type reference if any.
	std::shared_ptr <TypeContext> getContext();

	const char* getTypeString() override;

private:
	/// Generates this type reference entity.
	void onGenerate(BindingGenerator& generator) override;

	/// Increment the usage counter of the referred type.
	void onFirstUse() override;

	void onList(std::string_view indent) override;

	std::shared_ptr <TypeEntity> referred;
	std::shared_ptr <TypeContext> context;
};

}

#endif
