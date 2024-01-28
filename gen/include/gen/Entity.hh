#ifndef GEN_ENTITY_HH
#define GEN_ENTITY_HH

#include <vector>
#include <memory>
#include <string>
#include <string_view>

namespace gen
{

class BindingGenerator;

class Entity
{
public:
	Entity(std::string_view name);
	Entity();

	/// Adds a child entity
	///
	/// \return child The new child entity.
	void addChild(std::shared_ptr <Entity>&& child);
	
	/// Resolves an entity that has the given qualified
	/// name relative to this entity.
	///
	/// \param qualifiedName The qualified name of the entity delimited by dots.
	/// \param The resolved entity or nullptr.
	std::shared_ptr <Entity> resolve(std::string_view qualifiedName);

	/// Gets the entity name.
	///
	/// \return The entity name if any.
	const std::string& getName() const;

	/// This function is implemented by the given entity class.
	/// It calls the appropriate functions from a binding generator.
	///
	/// \param generator The BindingGenerator to call functions from.
	virtual void generate(BindingGenerator& generator) = 0;

	virtual const char* getTypeString() = 0;

	void list(unsigned depth = 1);

protected:
	virtual void onList(std::string_view indent);

	const std::string name;
	std::vector <std::shared_ptr <Entity>> children;
};

}

#endif
