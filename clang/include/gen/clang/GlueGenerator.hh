#ifndef GEN_CLANG_GLUE_GENERATOR_HH
#define GEN_CLANG_GLUE_GENERATOR_HH

#include <gen/BindingGenerator.hh>

#include <fstream>

namespace gen::clang
{

class Backend;

class GlueGenerator : public BindingGenerator
{
public:
	GlueGenerator(Backend& backend);

private:
	void generateClassBeginning(ClassEntity& entity) override;
	void generateClassEnding(ClassEntity& entity) override;
	void generateEnumBeginning(EnumEntity& entity) override;
	void generateEnumEnding(EnumEntity& entity) override;
	void generateEnumEntry(EnumEntryEntity& entity) override;
	void generateFunction(FunctionEntity& entity) override;
	void generateTypeReference(TypeReferenceEntity& entity) override;
	void generateNamedScopeBeginning(ScopeEntity& entity) override;
	void generateNamedScopeEnding(ScopeEntity& entity) override;
	void generateArgumentSeparator() override;

	std::ofstream file;
};

}

#endif
