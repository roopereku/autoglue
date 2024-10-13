#ifndef AUTOGLUE_ENTITY_HH
#define AUTOGLUE_ENTITY_HH

#include <autoglue/EntityContext.hh>

#include <vector>
#include <memory>
#include <string>
#include <string_view>

namespace ag
{

class BindingGenerator;

class Entity
{
public:
	enum class Type
	{
		Scope,
		Function,
		FunctionGroup,
		Type,
		TypeReference,
		EnumEntry
	};

	Entity(Type type, std::string_view name);
	virtual ~Entity();

	/// Adds a child entity
	///
	/// \return child The new child entity.
	void addChild(std::shared_ptr <Entity>&& child);

	/// Checks if this entity is the root entity.
	///
	/// \return True if this is the root entity.
	bool isRoot();

	/// Gets the parent entity.
	///
	/// \return The parent entity.
	virtual Entity& getParent() const;

	/// Initializes the context for this entity. This function does nothing
	/// if the context is already set.
	///
	/// \param ctx The context to set for this entity.
	void initializeContext(std::shared_ptr <EntityContext>&& ctx);

	/// Gets the context for this entity.
	///
	/// \return The context for this entity.
	std::shared_ptr <EntityContext> getContext() const;

	/// Resolves an entity that has the given qualified
	/// name relative to this entity.
	///
	/// \param qualifiedName The qualified name of the entity delimited by dots.
	/// \param The resolved entity or nullptr.
	virtual std::shared_ptr <Entity> resolve(std::string_view qualifiedName);

	/// Gets the entity name.
	///
	/// \return The entity name if any.
	virtual const std::string& getName() const;

	/// Returns a string containing the hierarchy leading up to this entity.
	///
	/// \param delimiter The delimiter to use between entity names. Defaults to underscore.
	/// \return String containing the hierarchy.
	virtual std::string getHierarchy(const std::string& delimiter = "_");

	/// This function invokes functions from the given BindingGenerator
	/// depending on the entity type.
	///
	/// \param generator The BindingGenerator to call functions from.
	void generate(BindingGenerator& generator);

	/// Recursively resets the flag indicating whether this entity has been exported.
	///
	/// \param generator The BindingGenerator that is used to initialize generation context.
	/// \param resetEntityContext If true, the context of this entity will be reset.
	void resetGenerationState(BindingGenerator& generator, bool resetEntityContext);

	/// Initializes a context for generation using a binding generator.
	///
	/// \param generator The BindingGenerator that does context initialization for this entity.
	void initializeGenerationContext(BindingGenerator& generator);

	/// Checks whether this entity has already been generated.
	///
	/// \return True if generation already has been done.
	bool isGenerated();

	/// Indicates that this entity has a new usage.
	/// Parent entities are implicitly used as well.
	void use();

	/// Indicates that all entities within this entity should be used.
	void useAll();

	/// Gets the type of this entity.
	///
	/// \return The type of this entity.
	Type getType();

	/// Gets the amount of usages.
	///
	/// \return The count of usages.
	unsigned getUsages();

	/// Disables any new usages.
	void disableNewUsages();

	virtual const char* getTypeString() = 0;

	void list(unsigned depth = 1);

protected:
	virtual void onList(std::string_view indent);

	virtual bool hasName(std::string_view str);

	/// This function is called upon the first time that the generation
	/// state for this entity is reset.
	virtual void onInitialize();

	/// This function is implemented by the given entity class.
	/// It calls the appropriate functions from a binding generator.
	///
	/// \param generator The BindingGenerator to call functions from.
	virtual void onGenerate(BindingGenerator& generator) = 0;

	/// This function is called when an entity is used for the first time.
	virtual void onFirstUse();

	/// Resets the usages of this entity.
	void resetUsages();

	/// Sets this entity as the parent of the given entity.
	void adoptEntity(Entity& entity);

	const std::string name;
	std::vector <std::shared_ptr <Entity>> children;

private:
	unsigned usages = 0;
	bool preventUsage = false;
	bool initialized = false;

	bool generated = false;
	Entity* parent = nullptr;

	Type type;
	std::shared_ptr <EntityContext> context;
};

}

#endif
