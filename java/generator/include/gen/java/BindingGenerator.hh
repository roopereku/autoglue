#ifndef GEN_JAVA_BINDING_GENERATOR_HH
#define GEN_JAVA_BINDING_GENERATOR_HH

#include <gen/BindingGenerator.hh>

#include <fstream>
#include <stack>

namespace gen::java
{

class BindingGenerator : public gen::BindingGenerator
{
public:
	BindingGenerator(Backend& backend);

private:
	void generateClassBeginning(ClassEntity& entity) override;
	void generateClassEnding(ClassEntity& entity) override;
	void generateEnumBeginning(EnumEntity& entity) override;
	void generateEnumEnding(EnumEntity& entity) override;
	void generateEnumEntry(EnumEntryEntity& entity) override;
	void generateFunction(FunctionEntity& entity) override;
	void generateParameter(ParameterEntity& entity) override;
	void generateNamedScopeBeginning(ScopeEntity& entity) override;
	void generateNamedScopeEnding(ScopeEntity& entity) override;

	std::ofstream file;
	std::stack <std::string> package;
};

}

#endif
