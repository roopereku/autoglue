#include <autoglue/FunctionEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/BindingGenerator.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>

namespace ag
{

FunctionEntity::FunctionEntity(std::shared_ptr <TypeReferenceEntity>&& returnType,
								bool overridable, bool overrides, bool interface)
	:	Entity(Entity::Type::Function, ""), returnType(std::move(returnType)),
		overridable(overridable), overrides(overrides), interface(interface)
{
}

void FunctionEntity::addParameter(std::shared_ptr <TypeReferenceEntity>&& param)
{
	addChild(std::move(param));
}

Entity& FunctionEntity::getParent() const
{
	// The parent that a user might care about is the parent of the containing group.
	return getGroup().getParent();
}

const std::string& FunctionEntity::getName() const
{
	return getGroup().getName();
}

FunctionGroupEntity& FunctionEntity::getGroup() const
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
	if(getType() == Type::Constructor && asPOD)
	{
		TypeReferenceEntity podRef("", PrimitiveEntity::getObjectHandle(), false);
		generator.generateTypeReference(podRef);

		return;
	}

	if(returnType)
	{
		if(asPOD && returnType->getType() != TypeEntity::Type::Primitive)
		{
			auto podRef = returnType->getAsPOD();
			generator.generateTypeReference(podRef);
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
		TypeReferenceEntity self(generator.getObjectHandleName(), PrimitiveEntity::getObjectHandle(), false);
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
			auto podRef = current->getAsPOD();
			generator.generateTypeReference(podRef);
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
	return getGroup().getType();
}

bool FunctionEntity::needsThisHandle()
{
	// TODO: Exclude static functions.
	return getType() == Type::MemberFunction ||
			getType() == Type::Destructor;
}

bool FunctionEntity::returnsValue()
{
	// The function returns a value if its a constructor or doesn't return void.
	return getType() == Type::Constructor ||
			(returnType && returnType->getReferredPtr() != PrimitiveEntity::getVoid());
}

bool FunctionEntity::isOverridable()
{
	return overridable;
}

bool FunctionEntity::isOverride()
{
	return overrides;
}

bool FunctionEntity::isInterface()
{
	return interface;
}

TypeReferenceEntity FunctionEntity::getReturnType()
{
	// Return a refrence to the parent class for constructors.
	if(getType() == Type::Constructor)
	{
		return TypeReferenceEntity("", static_cast <ClassEntity&> (getParent()).shared_from_this(), false);
	}

	assert(returnType);
	return *returnType;
}

std::string FunctionEntity::getBridgeName(bool shortened)
{
	if(shortened)
	{
		return getName() + std::to_string(overloadIndex);
	}

	return getHierarchy("_") + std::to_string(overloadIndex);
}

bool FunctionEntity::isClassMemberFunction()
{
	return getType() == Type::MemberFunction ||
			getType() == Type::Constructor ||
			getType() == Type::Destructor;
}

void FunctionEntity::setOverloadedOperator(OverloadedOperator overloaded, bool compound)
{
	overloadedOperator = overloaded;
	compoundOperator = compound;
}

FunctionEntity::OverloadedOperator FunctionEntity::getOverloadedOperator()
{
	return overloadedOperator;
}

bool FunctionEntity::overloadsCompoundOperator()
{
	return compoundOperator;
}

void FunctionEntity::setOverloadIndex(size_t index)
{
	overloadIndex = index;
}

const char* FunctionEntity::getTypeString()
{
	return "Function";
}

}
