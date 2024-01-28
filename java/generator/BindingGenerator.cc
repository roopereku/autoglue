#include <gen/java/BindingGenerator.hh>

namespace gen::java
{

BindingGenerator::BindingGenerator(Backend& backend)
	: gen::BindingGenerator(backend)
{
}

void BindingGenerator::generateClassBeginning(ClassEntity& entity)
{
}

void BindingGenerator::generateClassEnding(ClassEntity& entity)
{
}

void BindingGenerator::generateEnumBeginning(EnumEntity& entity)
{
}

void BindingGenerator::generateEnumEnding(EnumEntity& entity)
{
}

void BindingGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
}

void BindingGenerator::generateFunction(FunctionEntity& entity)
{
}

void BindingGenerator::generateParameter(ParameterEntity& entity)
{
}

void BindingGenerator::generateNamedScopeBeginning(ScopeEntity& entity)
{
}

void BindingGenerator::generateNamedScopeEnding(ScopeEntity& entity)
{
}

}
