#include <autoglue/ClassEntity.hh>
#include <autoglue/BindingGenerator.hh>

namespace ag
{

ClassEntity::ClassEntity(std::string_view name)
	: TypeEntity(name, Type::Class)
{
}

void ClassEntity::addBaseClass(std::shared_ptr <ClassEntity> base)
{
	// Make sure that the given base isn't already found.
	for(auto baseRef : baseClasses)
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

	baseClasses.emplace_back(base);
}

void ClassEntity::generateBaseClasses(BindingGenerator& generator)
{
	for(size_t i = 0; i < baseClasses.size(); i++)
	{
		// Only if the class definition still exists, generate it as a base class.
		if(!baseClasses[i].expired())
		{
			auto base = baseClasses[i].lock();
			generator.generateBaseClass(*base, i);

			// Add an argument separator for base classes that aren't the last one.
			if(i != baseClasses.size() - 1)
			{
				generator.generateArgumentSeparator();
			}
		}
	}
}

bool ClassEntity::hasBaseClasses()
{
	size_t bases = 0;

	for(auto base : baseClasses)
	{
		bases += !base.expired();
	}

	return bases > 0;
}

void ClassEntity::onGenerate(BindingGenerator& generator)
{
	// Begin the class.
	generator.generateClassBeginning(*this);
	generator.changeClassDepth(+1);

	// Generate the nested entities.
	for(auto child : children)
	{
		child->generate(generator);
	}

	// End the class.
	generator.changeClassDepth(-1);
	generator.generateClassEnding(*this);
}

void ClassEntity::onFirstUse()
{
	for(size_t i = 0; i < baseClasses.size(); i++)
	{
		if(!baseClasses[i].expired())
		{
			baseClasses[i].lock()->use();
		}
	}
}

const char* ClassEntity::getTypeString()
{
	return "Class";
}

}
