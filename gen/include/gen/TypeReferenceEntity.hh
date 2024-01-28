#ifndef GEN_TYPE_REFERENCE_ENTITY_HH
#define GEN_TYPE_REFERENCE_ENTITY_HH

#include <gen/Entity.hh>
#include <gen/ClassEntity.hh>

namespace gen
{

class TypeReferenceEntity : public Entity
{
public:
	TypeReferenceEntity(std::string_view name, std::shared_ptr <ClassEntity> type);

	/// Generates this type reference entity.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator) override;

	ClassEntity& getType();

	const char* getTypeString() override;

private:
	void onList(std::string_view indent) override;

	std::shared_ptr <ClassEntity> type;
};

}

#endif
