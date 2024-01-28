#include <gen/BindingGenerator.hh>
#include <gen/Backend.hh>

namespace gen
{

BindingGenerator::BindingGenerator(Backend& backend)
	: backend(backend)
{
}

void BindingGenerator::generateBindings()
{
	backend.getRoot().resolve("gen")->generate(*this);
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
