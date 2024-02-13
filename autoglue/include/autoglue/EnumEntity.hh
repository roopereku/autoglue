#ifndef AUTOGLUE_ENUM_ENTITY_HH
#define AUTOGLUE_ENUM_ENTITY_HH

#include <autoglue/TypeEntity.hh>

namespace ag
{

class EnumEntity : public TypeEntity
{
public:
	EnumEntity(std::string_view name);

	const char* getTypeString() override;

private:
	/// Generates enum values wrapped within an enum beginning and ending.
	void onGenerate(BindingGenerator& generator) override;

	/// Tells the most recent child that it is the last enum entry.
	void onFirstUse() override;
};

}

#endif
