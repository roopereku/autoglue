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
	void generateFunction(FunctionEntity& entity) override;
	void generateTypeReference(TypeReferenceEntity& entity) override;
	void generateArgumentSeparator() override;

	std::ofstream file;
	bool onlyParameterNames = false;
};

}

#endif
