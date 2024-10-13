#ifndef AUTOGLUE_SERIALIZER_HH
#define AUTOGLUE_SERIALIZER_HH

#include <autoglue/BindingGenerator.hh>

namespace ag
{

class Serializer : public BindingGenerator
{
public:
	/// Serialize the contents held by this serializer.
	/// This is the same as calling generateBindings.
	void serialize();

protected:
	Serializer(Backend& backend);

	virtual void beginElement(Entity& entity) = 0;
	virtual void beginNestingElement(Entity& entity);

	virtual void setReturnValue(FunctionEntity& entity) = 0;
	virtual void setReferredType(TypeAliasEntity& entity) = 0;
	virtual void addBaseType(TypeEntity& entity) = 0;

	virtual void endElement(Entity& entity) = 0;
	virtual void endNestingElement(Entity& entity);

	void generateClass(ClassEntity& entity) final override;
	void generateEnum(EnumEntity& entity) final override;
	void generateEnumEntry(EnumEntryEntity& entity) final override;
	void generateFunction(FunctionEntity& entity) final override;
	void generateTypeReference(TypeReferenceEntity& entity) final override;
	void generateTypeAlias(TypeAliasEntity& entity) final override;
	bool generateBaseType(TypeEntity& entity, size_t index) final override;
	void generateNamedScope(ScopeEntity& entity) final override;
	void generateArgumentSeparator() final override;
	bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target) final override;
};

}

#endif
