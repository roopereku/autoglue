#ifndef GEN_FUNCTION_GROUP_ENTITY_HH
#define GEN_FUNCTION_GROUP_ENTITY_HH

#include <gen/FunctionEntity.hh>

namespace gen
{

class FunctionGroupEntity : public Entity
{
public:
	FunctionGroupEntity(std::string_view name);

	const char* getTypeString() override;

private:
	/// Generates function overloads.
	void onGenerate(BindingGenerator& generator) override;

	/// Ignores functions which already exist.
	bool interceptNewChild(Entity& newChild) override;
};

}

#endif
