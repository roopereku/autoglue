#ifndef AUTOGLUE_CSHARP_BINDING_GENERATOR_HH
#define AUTOGLUE_CSHARP_BINDING_GENERATOR_HH

#include <autoglue/BindingGenerator.hh>

#include <string_view>
#include <fstream>
#include <string>
#include <stack>

namespace ag::csharp
{

class BindingGenerator : public ag::BindingGenerator
{
public:
	BindingGenerator(ag::Backend& backend, std::string_view libName);

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
	bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target) override;
	void generateBridgeCall(FunctionEntity& entity) override;
	void generateInterceptionFunction(FunctionEntity& entity, ClassEntity& parentClass) override;
	void generateInterceptionContext(ClassEntity& entity) override;
	std::string_view getObjectHandleName() override;

	bool hidesEntity(Entity& entity, Entity& containing);
	bool generateBridgeToCSharp(TypeReferenceEntity& entity);
	bool generateCSharpToBridge(TypeReferenceEntity& entity);

	void openFile(TypeEntity& entity);

	std::ofstream file;
	std::string libName;

	std::stack <std::string> namespaces;

	/// Used to only write parameter names when a bridge function is called.
	bool onlyParameterNames = false;

	bool convertStringType = false;
	bool inIntercept = false;
};

}

#endif
