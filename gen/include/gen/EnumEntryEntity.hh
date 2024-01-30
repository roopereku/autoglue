#ifndef GEN_ENUM_ENTRY_ENTITY_HH
#define GEN_ENUM_ENTRY_ENTITY_HH

#include <gen/Entity.hh>

namespace gen
{

class EnumEntryEntity : public Entity
{
public:
	EnumEntryEntity(std::string_view name, size_t value);

	/// Gets the value of the enum entry.
	///
	/// \return The value of the enum entry.
	size_t getValue();

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

	/// The value of the enum entry.
	size_t value;

	bool last = false;
};

}

#endif
