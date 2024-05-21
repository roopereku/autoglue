#include <autoglue/ClassEntity.hh>
#include <autoglue/BindingGenerator.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>

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

	// If this class is abstract, create a new "concrete type".
	// This is not stored in children in order for it to be generated
	// only on demand.
	if(abstract)
	{
		concreteType = std::make_shared <ClassEntity> ("ConcreteType");
		adoptEntity(*concreteType);
		
		concreteType->addBaseType(shared_from_this());

		addInterfaceOverridesToConcrete(concreteType);
		concreteType->use();
	}
}

bool ClassEntity::isAbstract()
{
	return abstract;
}

void ClassEntity::setAbstract()
{
	abstract = true;
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

const char* ClassEntity::getTypeString()
{
	return "Class";
}

void ClassEntity::addInterfaceOverridesToConcrete(std::shared_ptr <ClassEntity> concrete)
{
	// Collect new override function for any interface function that requires such.
	// These will make the concrete type instantiable.
	for(auto child : children)
	{
		if(child->getType() == Entity::Type::FunctionGroup)
		{
			auto& group = static_cast <FunctionGroupEntity&> (*child);
			auto result = group.createInterfaceOverrides();

			if(result)
			{
				auto overrideGroup = concrete->resolve(group.getName());

				// If there already was a function group for the overrides,
				// append the result to it.
				if(overrideGroup)
				{
					assert(overrideGroup->getType() == Entity::Type::FunctionGroup);
					std::static_pointer_cast <FunctionGroupEntity> (overrideGroup)->appendOverloads(result);
				}

				// Add a new group if there wasn't one yet.
				else
				{
					concrete->addNested(std::move(result));
				}
			}
		}
	}

	// In case a derived class doesn't implement an interface, collect
	// overrides for interfaces found in base classes.
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
				std::static_pointer_cast <ClassEntity> (base)->addInterfaceOverridesToConcrete(concrete);
			}
		}
	}
}

}
