#include <gen/BindingGenerator.hh>
#include <gen/Backend.hh>

namespace gen
{

BindingGenerator::BindingGenerator(Backend& backend)
	: backend(backend)
{
	// When a binding generator is initialized, reset generation state.
	backend.getRoot().resetGenerated();
}

void BindingGenerator::generateBindings()
{
	backend.getRoot().generate(*this);
}

void BindingGenerator::changeClassDepth(int amount)
{
	classDepth += amount;
}

unsigned BindingGenerator::getClassDepth()
{
	return classDepth;
}

}
