#ifndef AUTOGLUE_ENUM_ENTITY_HH
#define AUTOGLUE_ENUM_ENTITY_HH

#include <autoglue/TypeEntity.hh>
#include <autoglue/EnumEntryEntity.hh>

namespace ag
{

/// EnumEntity represents a collection of human readable names that
/// correspond to some type of a value (e.g. Integers).
class EnumEntity : public TypeEntity
{
public:
	enum class Format
	{
		Integer
	};

	EnumEntity(std::string_view name, Format format);

	/// Adds a new entry to this enum.
	///
	/// \param entry The enum entry to add.
	void addEntry(std::shared_ptr <EnumEntryEntity>&& entry);

	/// Generates the values of this enum.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generateValues(BindingGenerator& generator);

	/// Gets the format used for the contained enum entries.
	///
	/// \return The format used for the contained enum entries.
	Format getFormat();

	const char* getTypeString() override;

private:
	/// Generates enum values wrapped within an enum beginning and ending.
	void onGenerate(BindingGenerator& generator) override;

	/// Tells the most recent child that it is the last enum entry.
	void onFirstUse() override;

	Format format;
};

}

#endif
