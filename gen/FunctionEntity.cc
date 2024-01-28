#include <gen/FunctionEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

FunctionEntity::FunctionEntity(std::string_view name)
	: Entity(name)
{
}

void FunctionEntity::generate(BindingGenerator& generator)
{
	generator.generateFunction(*this);
}

const char* FunctionEntity::getTypeString()
{
	return "Function";
}

}
