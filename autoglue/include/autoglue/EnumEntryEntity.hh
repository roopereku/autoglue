#ifndef AUTOGLUE_ENUM_ENTRY_ENTITY_HH
#define AUTOGLUE_ENUM_ENTRY_ENTITY_HH

#include <autoglue/Entity.hh>

namespace ag
{

class EnumEntryEntity : public Entity
{
public:
	EnumEntryEntity(std::string_view name, std::string&& value);

	/// Gets the value of the enum entry.
	///
	/// \return The value of the enum entry.
	std::string_view getValue();

	/// Checks if this enum entry is the last one of an enum.
	///
	/// \return True if this is the last enum entry of an enum.
	bool isLast();

	const char* getTypeString() override;

	/// EnumEntity needs access to "last".
	friend class EnumEntity;

private:
	/// Generates an enum entry.
	void onGenerate(BindingGenerator& generator) override;

	/// The value and format of the enum entry.
	std::string value;

	bool last = false;
};

}

#endif
