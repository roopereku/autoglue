#ifndef AUTOGLUE_ENUM_ENTITY_HH
#define AUTOGLUE_ENUM_ENTITY_HH

#include <autoglue/TypeEntity.hh>
#include <autoglue/EnumEntryEntity.hh>

namespace ag
{

class EnumEntity : public TypeEntity
{
public:
	EnumEntity(std::string_view name);

	/// Adds a new entry to this enum.
	///
	/// \param entry The enum entry to add.
	void addEntry(std::shared_ptr <EnumEntryEntity>&& entry);

	/// Generates the values of this enum.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateValues(BindingGenerator& generator);

	const char* getTypeString() override;

private:
	/// Generates enum values wrapped within an enum beginning and ending.
	void onGenerate(BindingGenerator& generator) override;

	/// Tells the most recent child that it is the last enum entry.
	void onFirstUse() override;
};

}

#endif
