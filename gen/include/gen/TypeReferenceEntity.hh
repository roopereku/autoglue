#ifndef GEN_TYPE_REFERENCE_ENTITY_HH
#define GEN_TYPE_REFERENCE_ENTITY_HH

#include <gen/Entity.hh>
#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>
#include <gen/TypeContext.hh>

namespace gen
{

class TypeReferenceEntity : public Entity
{
public:
	TypeReferenceEntity(std::string_view name, std::shared_ptr <ClassEntity> classType);
	TypeReferenceEntity(std::string_view name, std::shared_ptr <EnumEntity> enumType);

	enum class Type
	{
		Class,
		Enum
	};

	/// Gets the type of referenced type entity.
	///
	/// \return The type of the referenced type entity.
	Type getType();

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

	Type type;
	std::shared_ptr <Entity> referred;
	std::shared_ptr <TypeContext> context;
};

}

#endif
