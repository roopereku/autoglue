#ifndef GEN_PARAMETER_ENTITY_HH
#define GEN_PARAMETER_ENTITY_HH

#include <gen/Entity.hh>
#include <gen/ClassEntity.hh>

namespace gen
{

class ParameterEntity : public Entity
{
public:
	ParameterEntity(std::string_view name, std::shared_ptr <ClassEntity> type);

	const char* getTypeString() override;

private:
	void onList(std::string_view indent) override;

	std::shared_ptr <ClassEntity> type;
};

}

#endif
