#include <autoglue/BindingGenerator.hh>
#include <autoglue/Backend.hh>

namespace ag
{

BindingGenerator::BindingGenerator(Backend& backend)
	: backend(backend)
{
}

void BindingGenerator::generateBindings()
{
	backend.getRoot().resetGenerated();
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

void BindingGenerator::generateClass(ClassEntity&) {}
void BindingGenerator::generateEnumBeginning(EnumEntity&) {}
void BindingGenerator::generateEnumEnding(EnumEntity&) {}
void BindingGenerator::generateEnumEntry(EnumEntryEntity&) {}
void BindingGenerator::generateFunction(FunctionEntity&) {}
void BindingGenerator::generateTypeReference(TypeReferenceEntity&) {}
void BindingGenerator::generateTypeAlias(TypeAliasEntity&) {}
void BindingGenerator::generateBaseClass(ClassEntity&, size_t) {}
void BindingGenerator::generateNamedScopeBeginning(ScopeEntity&) {}
void BindingGenerator::generateNamedScopeEnding(ScopeEntity&) {}
void BindingGenerator::generateArgumentSeparator() {}

}
