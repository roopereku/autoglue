#ifndef GEN_CLASS_ENTITY_HH
#define GEN_CLASS_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class ClassEntity : public Entity
{
public:
	ClassEntity(std::string_view name);

	const char* getTypeString() override;

private:
	/// Generates the child entities wrapped inside class beginning and ending.
	void onGenerate(BindingGenerator& generator) override;
};

}

#endif
