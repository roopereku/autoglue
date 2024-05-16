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
void BindingGenerator::generateEnum(EnumEntity&) {}
void BindingGenerator::generateEnumEntry(EnumEntryEntity&) {}
void BindingGenerator::generateFunction(FunctionEntity&) {}
void BindingGenerator::generateTypeReference(TypeReferenceEntity&) {}
void BindingGenerator::generateTypeAlias(TypeAliasEntity&) {}
void BindingGenerator::generateBaseType(TypeEntity&, size_t) {}
void BindingGenerator::generateNamedScope(ScopeEntity&) {}
void BindingGenerator::generateArgumentSeparator() {}
bool BindingGenerator::generateReturnStatement(TypeReferenceEntity&, FunctionEntity&) { return false; }
void BindingGenerator::generateBridgeCall(FunctionEntity&) {}

std::string_view BindingGenerator::getObjectHandleName()
{
	return "mObjectHandle";
}

}
