#include <gen/ClassEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

ClassEntity::ClassEntity(std::string_view name)
	: TypeEntity(name, Type::Class)
{
}

void ClassEntity::onGenerate(BindingGenerator& generator)
{
	// Begin the class.
	generator.generateClassBeginning(*this);
	generator.changeClassDepth(+1);

	// Generate the nested entities.
	for(auto child : children)
	{
		child->generate(generator);
	}

	// End the class.
	generator.changeClassDepth(-1);
	generator.generateClassEnding(*this);
}

const char* ClassEntity::getTypeString()
{
	return "Class";
}

}
