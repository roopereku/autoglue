#ifndef AUTOGLUE_JAVA_BINDING_GENERATOR_HH
#define AUTOGLUE_JAVA_BINDING_GENERATOR_HH

#include <autoglue/BindingGenerator.hh>

#include <string_view>
#include <fstream>
#include <string>
#include <stack>

namespace ag::java
{

class BindingGenerator : public ag::BindingGenerator
{
public:
	BindingGenerator(Backend& backend, std::string_view packagePrefix);

private:
	void generateClass(ClassEntity& entity) override;
	void generateEnum(EnumEntity& entity) override;
	void generateEnumEntry(EnumEntryEntity& entity) override;
	void generateFunction(FunctionEntity& entity) override;
	void generateTypeReference(TypeReferenceEntity& entity) override;
	void generateTypeAlias(TypeAliasEntity& entity) override;
	void generateBaseType(TypeEntity& entity, size_t index) override;
	void generateNamedScope(ScopeEntity& entity) override;
	void generateArgumentSeparator() override;

	void generateTyperefJNI(TypeReferenceEntity& entity);
	void generateTyperefJava(TypeReferenceEntity& entity);
	bool handleReturnValue(TypeReferenceEntity& entity);

	void openFile(Entity& entity);

	std::ofstream file;
	std::ofstream jni;

	std::stack <std::string> package;
	std::string packagePrefix;

	/// Used to only write parameter names when a bridge function is called.
	bool onlyParameterNames = false;

	/// Used to indicate that type parameters should be written to the JNI.
	bool inJni = false;

	/// Used to indicate that type parameters should be written in the bridge format
	bool inExtern = false;
};

}

#endif
