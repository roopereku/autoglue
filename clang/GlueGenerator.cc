#include <gen/clang/GlueGenerator.hh>
#include <gen/clang/Backend.hh>

namespace gen::clang
{

GlueGenerator::GlueGenerator(Backend& backend)
	: BindingGenerator(backend), file("glue.cc")
{
}

void GlueGenerator::generateClassBeginning(ClassEntity& entity)
{
}

void GlueGenerator::generateClassEnding(ClassEntity& entity)
{
}

void GlueGenerator::generateEnumBeginning(EnumEntity& entity)
{
}

void GlueGenerator::generateEnumEnding(EnumEntity& entity)
{
}

void GlueGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
}

void GlueGenerator::generateFunction(FunctionEntity& entity)
{
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
}

void GlueGenerator::generateNamedScopeBeginning(ScopeEntity& entity)
{
}

void GlueGenerator::generateNamedScopeEnding(ScopeEntity& entity)
{
}

void GlueGenerator::generateArgumentSeparator()
{
}

}
