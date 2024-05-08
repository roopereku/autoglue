#include <autoglue/FunctionEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/BindingGenerator.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>

namespace ag
{

FunctionEntity::FunctionEntity(std::string_view name, Type type,
								std::shared_ptr <TypeReferenceEntity>&& returnType,
								bool overridable, bool interface)
	:	Entity(name), type(type), returnType(std::move(returnType)),
		overridable(overridable), interface(interface)
{
}

Entity& FunctionEntity::getParent()
{
	// The parent that a user might care about is the parent of the containing group.
	return getGroup().getParent();
}

FunctionGroupEntity& FunctionEntity::getGroup()
{
	return static_cast <FunctionGroupEntity&> (Entity::getParent());
}

std::string FunctionEntity::getHierarchy(const std::string& delimiter)
{
	return getGroup().getHierarchy(delimiter);
}

void FunctionEntity::onGenerate(BindingGenerator& generator)
{
	// TODO: Until support code for type extensions are generated, don't generate
	// constructors for abstract classes since they likely cannot be instantiated.
	if(getType() == Type::Constructor &&
		static_cast <ClassEntity&> (getParent()).isAbstract())
	{
		return;
	}

	generator.generateFunction(*this);
}

void FunctionEntity::onFirstUse()
{
	if(returnType)
	{
		returnType->use();
	}
}

void FunctionEntity::generateReturnType(BindingGenerator& generator, bool asPOD)
{
	// If this function is a constructor, generate an object handle.
	if(type == Type::Constructor && asPOD)
	{
		TypeReferenceEntity podRef("", PrimitiveEntity::getObjectHandle(), false);
		generator.generateTypeReference(podRef);

		return;
	}

	if(returnType)
	{
		if(asPOD && returnType->getType() != TypeEntity::Type::Primitive)
		{
			generatePOD(generator, *returnType);
		}

		else
		{
			generator.generateTypeReference(*returnType);
		}
	}
}

void FunctionEntity::generateParameters(BindingGenerator& generator, bool asPOD, bool includeSelf)
{
	// If the self parameter should be included, export it for member functions and destructors.
	if(includeSelf && needsThisHandle())
	{
		TypeReferenceEntity self("objectHandle", PrimitiveEntity::getObjectHandle(), false);
		generator.generateTypeReference(self);

		// If there are more parameters, add a separator.
		if(getParameterCount() > 0)
		{
			generator.generateArgumentSeparator();
		}
	}

	for(size_t i = 0; i < children.size(); i++)
	{
		auto current = std::static_pointer_cast <TypeReferenceEntity> (children[i]);

		if(asPOD && current->getType() != TypeEntity::Type::Primitive)
		{
			generatePOD(generator, *current);
		}

		else
		{
			generator.generateTypeReference(*current);
		}

		// Add an argument separator for parameters that aren't the last one.
		if(i != children.size() - 1)
		{
			generator.generateArgumentSeparator();
		}
	}
}

size_t FunctionEntity::getParameterCount(bool includeSelf)
{
	return children.size() + (includeSelf && needsThisHandle());
}

TypeReferenceEntity& FunctionEntity::getParameter(size_t index)
{
	return static_cast <TypeReferenceEntity&> (*children[index]);
}

FunctionEntity::Type FunctionEntity::getType()
{
	return type;
}

bool FunctionEntity::needsThisHandle()
{
	// TODO: Exclude static functions.
	return type == Type::MemberFunction ||
			type == Type::Destructor;
}

bool FunctionEntity::returnsValue()
{
	// The function returns a value if its a constructor or doesn't return void.
	return type == Type::Constructor ||
			(returnType && returnType->getReferredPtr() != PrimitiveEntity::getVoid());
}

bool FunctionEntity::isOverridable()
{
	return overridable;
}

bool FunctionEntity::isInterface()
{
	return interface;
}

TypeReferenceEntity& FunctionEntity::getReturnType()
{
	assert(returnType);
	return *returnType;
}

const char* FunctionEntity::getTypeString()
{
	return "Function";
}

void FunctionEntity::generatePOD(BindingGenerator& generator, TypeReferenceEntity ref)
{
	switch(ref.getType())
	{
		case TypeEntity::Type::Class:
		{
			TypeReferenceEntity podRef(ref.getName(), PrimitiveEntity::getObjectHandle(), ref.isReference());
			generator.generateTypeReference(podRef);

			break;
		}

		case TypeEntity::Type::Enum:
		{
			TypeReferenceEntity podRef(ref.getName(), PrimitiveEntity::getInteger(), ref.isReference());
			generator.generateTypeReference(podRef);

			break;
		}

		case TypeEntity::Type::Alias:
		{
			TypeReferenceEntity podRef(ref.getName(), ref.getAliasType().getUnderlying(true), ref.isReference());
			generatePOD(generator, podRef);

			break;
		}

		case TypeEntity::Type::Primitive:
		{
			generator.generateTypeReference(ref);
			break;
		}
	}
}

}
