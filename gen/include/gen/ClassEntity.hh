#ifndef GEN_CLASS_ENTITY_HH
#define GEN_CLASS_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class ClassEntity : public Entity
{
public:
	ClassEntity(std::string_view name);

	/// Generates a class body including nested entities.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator) override;

	const char* getTypeString() override;
};

}

#endif
