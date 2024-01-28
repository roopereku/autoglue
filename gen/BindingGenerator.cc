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
	backend.getRoot().generate(*this);
}

}
