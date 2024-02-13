#ifndef AUTOGLUE_CLANG_GLUE_GENERATOR_HH
#define AUTOGLUE_CLANG_GLUE_GENERATOR_HH

#include <autoglue/BindingGenerator.hh>

#include <fstream>

namespace ag::clang
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
