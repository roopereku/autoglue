#ifndef GEN_TYPE_ALIAS_ENTITY_HH
#define GEN_TYPE_ALIAS_ENTITY_HH

#include <gen/TypeEntity.hh>

namespace gen
{

class TypeAliasEntity : public TypeEntity
{
public:
	TypeAliasEntity(std::string_view name, std::shared_ptr <TypeEntity> underlying);

	/// Gets the underlying type entity.
	///
	/// \return The underlying type entity or a null.
	std::shared_ptr <TypeEntity> getUnderlying();

	const char* getTypeString() override;

private:
	/// Makes sure that the underlying type is used.
	void onFirstUse() override;

	/// Calls generateTypeAlias for this entity.
	void onGenerate(BindingGenerator& generator) override;

	std::weak_ptr <TypeEntity> underlying;
};

}

#endif
