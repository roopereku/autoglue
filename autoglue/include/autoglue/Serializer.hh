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

	/// Used to begin an element that can contain other elements.
	virtual void beginElement(Entity& entity) = 0;

	/// Used to end an element that can contain other elements.
	virtual void endElement(Entity& entity) = 0;

	/// Used to write an element that cannot contain other elements.
	virtual void writeElement(Entity& entity) = 0;

	virtual void setReturnValue(FunctionEntity& entity) = 0;
	virtual void setReferredType(TypeAliasEntity& entity) = 0;
	virtual void addBaseType(TypeEntity& entity) = 0;

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
