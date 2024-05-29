#include <autoglue/ClassEntity.hh>
#include <autoglue/BindingGenerator.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>
#include <memory>

namespace ag
{

ClassEntity::ClassEntity(std::string_view name)
	: TypeEntity(name, Type::Class)
{
}

void ClassEntity::addNested(std::shared_ptr <Entity>&& nested)
{
	addChild(std::move(nested));
}

void ClassEntity::addBaseType(std::shared_ptr <TypeEntity> base)
{
	// Make sure that the given base isn't already found.
	for(auto baseRef : baseTypes)
	{
		if(baseRef.expired())
		{
			continue;
		}

		if(baseRef.lock() == base)
		{
			return;
		}
	}

	baseTypes.emplace_back(base);

	// If the base type is a class, tell it that this class derives it.
	if(base->getType() == Type::Class)
	{
		std::static_pointer_cast <ClassEntity> (base)->derivedClasses.emplace_back(
			shared_from_this()
		);
	}
}

void ClassEntity::generateBaseTypes(BindingGenerator& generator)
{
	for(size_t i = 0; i < baseTypes.size(); i++)
	{
		// Only if the type definition still exists, generate it as a base type.
		if(!baseTypes[i].expired())
		{
			auto base = baseTypes[i].lock();
			generator.generateBaseType(*base, i);

			// Add an argument separator for base types that aren't the last one.
			if(i != baseTypes.size() - 1)
			{
				generator.generateArgumentSeparator();
			}
		}
	}
}

bool ClassEntity::hasBaseTypes()
{
	size_t bases = 0;

	for(auto base : baseTypes)
	{
		bases += !base.expired();
	}

	return bases > 0;
}

size_t ClassEntity::getBaseTypeCount()
{
	return baseTypes.size();
}

TypeEntity& ClassEntity::getBaseType(size_t index)
{
	auto weak = baseTypes[index];
	assert(!weak.expired());

	return *weak.lock();
}

size_t ClassEntity::getDerivedCount()
{
	return derivedClasses.size();
}

ClassEntity& ClassEntity::getDerived(size_t index)
{
	assert(index < derivedClasses.size());
	assert(!derivedClasses[index].expired());

	return *derivedClasses[index].lock();
}

void ClassEntity::generateNested(BindingGenerator& generator)
{
	// Generate the nested entities.
	for(auto child : children)
	{
		child->generate(generator);
	}
}

void ClassEntity::onGenerate(BindingGenerator& generator)
{
	generator.changeClassDepth(+1);
	generator.generateClass(*this);
	generator.changeClassDepth(-1);
}

void ClassEntity::onFirstUse()
{
	// Only use the nested entities for concrete types.
	if(isConcrete)
	{
		return;
	}

	// In order to make this class type instantiable, make sure
	// that its constructors are used.
	auto constructors = resolve("Constructor");
	if(constructors)
	{
		constructors->use();
	}

	// In order to clean up any allocated memory, make sure
	// that the class destructors are used.
	auto destructors = resolve("Destructor");
	if(destructors)
	{
		destructors->use();
	}

	for(size_t i = 0; i < baseTypes.size(); i++)
	{
		if(!baseTypes[i].expired())
		{
			baseTypes[i].lock()->use();
		}
	}

	if(!concreteType)
	{
		auto concrete = std::make_shared <ClassEntity> ("ConcreteType");
		concrete->isConcrete = true;
		adoptEntity(*concrete);
		
		concrete->addBaseType(shared_from_this());
		addOverridesToConcrete(concrete);

		// Only save the concrete type if overrides were added to it.
		if(!concrete->children.empty())
		{
			concreteType = std::move(concrete);
		}
	}
}

bool ClassEntity::isAbstract()
{
	return abstract;
}

void ClassEntity::generateConcreteType(BindingGenerator& generator)
{
	if(concreteType)
	{
		concreteType->generate(generator);
		concreteType->resetGenerated();
	}
}

std::shared_ptr <ClassEntity> ClassEntity::getConcreteType()
{
	return concreteType;
}

bool ClassEntity::isConcreteType()
{
	return isConcrete;
}

void ClassEntity::generateInterceptionFunctions(BindingGenerator& generator)
{
	if(concreteType)
	{
		for(auto child : concreteType->children)
		{
			assert(child->getType() == Entity::Type::FunctionGroup);
			auto& group = static_cast <FunctionGroupEntity&> (*child);

			for(size_t i = 0; i < group.getOverloadCount(); i++)
			{
				auto overridden = group.getOverload(i).getOverridden();
				assert(overridden);

				if(overridden->getUsages() > 0 && overridden->isOverridable())
				{
					generator.generateInterceptionFunction(group.getOverload(i), *this);
				}
			}
		}
	}
}

void ClassEntity::generateInterceptionContext(BindingGenerator& generator)
{
	if(concreteType)
	{
		generator.generateInterceptionContext(*this);
	}
}

bool ClassEntity::matchType(Entity& entity)
{
	return entity.getType() == Entity::Type::Type &&
			static_cast <TypeEntity&> (entity).getType() == Type::Class;
}

const char* ClassEntity::getTypeString()
{
	return "Class";
}

void ClassEntity::addOverridesToConcrete(std::shared_ptr <ClassEntity> concrete)
{
	for(auto child : children)
	{
		if(child->getType() == Entity::Type::FunctionGroup)
		{
			auto& group = static_cast <FunctionGroupEntity&> (*child);

			// TODO: Implement virtual destructors?
			if(group.getType() == FunctionEntity::Type::Destructor)
			{
				continue;
			}

			std::shared_ptr <FunctionGroupEntity> overrideGroup;
			auto resolved = concrete->resolve(group.getName());

			if(!resolved)
			{
				overrideGroup = std::make_shared <FunctionGroupEntity> (
					group.getName(), group.getType()
				);
			}

			else
			{
				assert(resolved->getType() == Entity::Type::FunctionGroup);
				overrideGroup = std::static_pointer_cast <FunctionGroupEntity> (resolved);
			}

			for(size_t i = 0; i < group.getOverloadCount(); i++)
			{
				// In order to make abstract types properly instantiable, implicitly
				// use interface functions that need to be overridden.
				if(group.getOverload(i).isInterface())
				{
					group.getOverload(i).use();
				}

				// If the current function is an override or an overridable, an override
				// representing the function will be returned.
				auto overrideEntity = group.getOverload(i).createOverride();

				// If the override function can be added to the concrete type, it means that it
				// previously didn't have a function of the given signature.
				if(overrideEntity && overrideGroup->addOverload(std::move(overrideEntity)))
				{
					// If the current function is an interface that doesn't have an override
					// in the concrete type, the class should be made abstract as a
					// foreign language has to implement the function.
					if(group.getOverload(i).isInterface())
					{
						auto& parent = concrete->getParent();

						assert(parent.getType() == Entity::Type::Type);
						assert(static_cast <TypeEntity&> (parent).getType() == TypeEntity::Type::Class);

						static_cast <ClassEntity&> (parent).abstract = true;
					}
				}
			}

			// If the function group didn't exist previously, add it.
			if(!resolved && overrideGroup->getOverloadCount() > 0)
			{
				concrete->addNested(std::move(overrideGroup));
			}
		}
	}

	// In case a base class has overridable functions, add them too to the concrete type.
	// Doing so will also catch unimplemented interfaces which make a class abstract.
	for(auto weakBase : baseTypes)
	{
		if(!weakBase.expired())
		{
			auto base = weakBase.lock();

			// If the base type is an alias, use the underlying type instead.
			if(base->getType() == Type::Alias)
			{
				base = std::static_pointer_cast <TypeAliasEntity> (base)->getUnderlying(true);
				assert(base);
			}

			// Only if the base type is a class, add overrides for its interfaces
			// to the given concrete type.
			if(base->getType() == Type::Class)
			{
				std::static_pointer_cast <ClassEntity> (base)->addOverridesToConcrete(concrete);
			}
		}
	}
}

void ClassEntity::onInitialize()
{
	if(concreteType)
	{
		size_t used = 0;

		// Now that we should know what is used and what is not, let's go through
		// each override within the concrete type and use those which refer to
		// a used overridable function. When a concrete type is being generated,
		// this will make generateNested generate overrides for any overridable functions
		// within the concrete type.
		for(auto child : concreteType->children)
		{
			assert(child->getType() == Entity::Type::FunctionGroup);
			auto& group = static_cast <FunctionGroupEntity&> (*child);

			for(size_t i = 0; i < group.getOverloadCount(); i++)
			{
				auto overridden = group.getOverload(i).getOverridden();
				assert(overridden);

				if(overridden->isOverridable() && overridden->getUsages() > 0)
				{
					group.getOverload(i).use();
					used++;
				}
			}
		}

		// If nothing is used, the concrete type shouldn't be generated at all.
		if(used == 0)
		{
			concreteType = nullptr;
		}
	}
}

}
