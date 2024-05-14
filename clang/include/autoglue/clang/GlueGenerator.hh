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
	void generateTypeReference(TypeReferenceEntity& entity) override;
	void generateFunction(FunctionEntity& entity) override;
	void generateNamedScope(ScopeEntity& entity) override;
	void generateClass(ClassEntity& entity) override;
	void generateArgumentSeparator() override;
	std::string_view getObjectHandleName() override;
	bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target) override;

	std::string getSelfType(FunctionEntity& entity);

	std::ofstream file;
	bool onlyParameterNames = false;
};

}

#endif
