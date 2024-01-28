#include <gen/ClassEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

ClassEntity::ClassEntity(std::string_view name)
	: Entity(name)
{
}

void ClassEntity::generate(BindingGenerator& generator)
{
	// Begin the class.
	generator.generateClassBeginning(*this);

	// Generate the nested entities.
	for(auto child : children)
	{
		child->generate(generator);
	}

	// End the class.
	generator.generateClassEnding(*this);
}

const char* ClassEntity::getTypeString()
{
	return "Class";
}

}
