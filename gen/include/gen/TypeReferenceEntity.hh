#ifndef GEN_TYPE_REFERENCE_ENTITY_HH
#define GEN_TYPE_REFERENCE_ENTITY_HH

#include <gen/Entity.hh>
#include <gen/ClassEntity.hh>
#include <gen/TypeContext.hh>

namespace gen
{

class TypeReferenceEntity : public Entity
{
public:
	TypeReferenceEntity(std::string_view name, std::shared_ptr <ClassEntity> type);

	ClassEntity& getType();

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

	std::shared_ptr <ClassEntity> type;
	std::shared_ptr <TypeContext> context;
};

}

#endif
