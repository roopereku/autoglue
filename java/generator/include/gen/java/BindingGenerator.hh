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
	void generateTypeReference(TypeReferenceEntity& entity) override;
	void generateNamedScopeBeginning(ScopeEntity& entity) override;
	void generateNamedScopeEnding(ScopeEntity& entity) override;
	void generateArgumentSeparator() override;

	bool isTrivialType(ClassEntity& entity);

	std::string getJniType(ClassEntity& entity);
	std::string getHierarchyPrefix(Entity& entity);

	std::ofstream file;
	std::ofstream jni;
	std::stack <std::string> package;

	/// Used to only write parameter names when a bridge function is called.
	bool onlyParameterNames = false;

	/// Used to indicate that type parameters should be written to the JNI.
	bool inJni = false;
};

}

#endif
