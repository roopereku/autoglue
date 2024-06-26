#ifndef AUTOGLUE_TYPE_ALIAS_ENTITY_HH
#define AUTOGLUE_TYPE_ALIAS_ENTITY_HH

#include <autoglue/TypeEntity.hh>

namespace ag
{

class TypeAliasEntity : public TypeEntity
{
public:
	TypeAliasEntity(std::string_view name, std::shared_ptr <TypeEntity> underlying);

	/// Gets the underlying type entity.
	///
	/// \param recursive If true, recursively gets underlying type of nested aliases.
	/// \return The underlying type entity or a null.
	std::shared_ptr <TypeEntity> getUnderlying(bool recursive = false);

	/// Resolves an entity from the underlying type pointed at by this type alias.
	///
	/// \param qualifiedName The qualified name of the entity delimited by dots.
	/// \param The resolved entity or nullptr.
	std::shared_ptr <Entity> resolve(std::string_view qualifiedName) override;

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
