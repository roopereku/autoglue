#include <autoglue/FunctionEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/BindingGenerator.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>

namespace ag
{

FunctionEntity::FunctionEntity(std::shared_ptr <TypeReferenceEntity>&& returnType,
								bool overridable, bool overrides, bool interface, bool staticFunction)
	:	Entity(Entity::Type::Function, ""), returnType(std::move(returnType)),
		overridable(overridable || interface), overrides(overrides),
		interface(interface), staticFunction(staticFunction)
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
	generator.generateFunction(*this);
}

void FunctionEntity::onFirstUse()
{
	if(returnType)
	{
		returnType->use();
	}

	for(auto param : children)
	{
		param->use();
	}

	// If this function is an override, associate the overridden function with it.
	if(!isConcreteOverride() && isOverride())
	{
		auto& parent = getParent();	

		if(ClassEntity::matchType(parent))
		{
			auto result = static_cast <ClassEntity&> (parent).findOverridableFromBase(*this);

			if(result)
			{
				// In order for an override to exist, implicitly use the overridden function.
				result->use();
				overridden = result;
			}
		}
	}
}

void FunctionEntity::generateReturnType(BindingGenerator& generator, bool asPOD, bool preferUnderlying)
{
	auto retType = getReturnType(asPOD, preferUnderlying);
	generator.generateTypeReference(retType);
}

bool FunctionEntity::generateReturnStatement(BindingGenerator& generator, bool asPOD, bool preferUnderlying)
{
	if(returnsValue())
	{
		auto retType = getReturnType(asPOD, preferUnderlying);
		return generator.generateReturnStatement(retType, *this);
	}

	return false;
}

void FunctionEntity::generateBridgeCall(BindingGenerator& generator)
{
	generator.generateBridgeCall(*this);
}

void FunctionEntity::generateParameters(BindingGenerator& generator, bool asPOD, bool includeSelf, bool preferUnderlying)
{
	// If the self parameter should be included, export it for member functions and destructors.
	if(includeSelf && needsThisHandle())
	{
		auto original = getSelfType(generator.getObjectHandleName());
		auto selfType = getTypeReference(original, true, preferUnderlying);

		generator.generateTypeReference(selfType);

		// If there are more parameters, add a separator.
		if(getParameterCount() > 0)
		{
			generator.generateArgumentSeparator();
		}
	}

	for(size_t i = 0; i < children.size(); i++)
	{
		auto current = std::static_pointer_cast <TypeReferenceEntity> (children[i]);
		auto param = getTypeReference(*current, asPOD, preferUnderlying);

		generator.generateTypeReference(param);

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
	return !staticFunction &&
			(getType() == Type::MemberFunction ||
			getType() == Type::Destructor);
}

bool FunctionEntity::returnsValue()
{
	// The function returns a value if its a constructor or doesn't return void.
	return getType() == Type::Constructor ||
			!(returnType->isPrimitive() &&
			 returnType->getPrimitiveType().getType() == PrimitiveEntity::Type::Void);
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

TypeReferenceEntity FunctionEntity::getReturnType(bool asPOD, bool preferUnderlying)
{
	// Return a reference to the parent class for constructors.
	if(getType() == Type::Constructor)
	{
		auto selfType = getSelfType("");
		return getTypeReference(selfType, asPOD, preferUnderlying);
	}

	assert(returnType);
	return getTypeReference(*returnType, asPOD, preferUnderlying);
}

TypeReferenceEntity FunctionEntity::getTypeReference(TypeReferenceEntity& ref, bool asPOD, bool preferUnderlying)
{
	if(preferUnderlying)
	{
		auto underlying = ref.getUnderlying();
		return getTypeReference(underlying, asPOD, false);
	}

	else if(asPOD)
	{
		return ref.getAsPOD();
	}

	return ref;
}

TypeReferenceEntity FunctionEntity::getSelfType(std::string_view name)
{
	assert(ClassEntity::matchType(getParent()));
	return TypeReferenceEntity(name, static_cast <ClassEntity&> (getParent()).shared_from_this(), false);
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

std::shared_ptr <FunctionEntity> FunctionEntity::createOverride(bool makeInterface, bool inConcrete)
{
	if(isOverridable() || isOverride())
	{
		auto ret = returnType;
		auto functionOverride = std::make_shared <FunctionEntity> (*this);

		functionOverride->overridable = makeInterface;
		functionOverride->interface = makeInterface;
		functionOverride->overrides = true;

		functionOverride->overridden = shared_from_this();
		functionOverride->concreteOverride = inConcrete;
		functionOverride->resetUsages();

		// Interfaces are used implicitly.
		if(makeInterface)
		{
			functionOverride->use();
		}

		return functionOverride;
	}

	return nullptr;
}

void FunctionEntity::setProtected()
{
	protectedFunction = true;
}

bool FunctionEntity::isProtected()
{
	return protectedFunction;
}

bool FunctionEntity::shouldPrepareClass()
{
	if(getType() == Type::Constructor)
	{
		auto& parent = getParent();
		assert(parent.getType() == Entity::Type::Type);
		assert(static_cast <TypeEntity&> (parent).getType() == TypeEntity::Type::Class);

		// If a concrete type exists containing overridable methods, further class
		// preparation should be done by this constructor function.
		return static_cast <bool> (static_cast <ClassEntity&> (parent).getConcreteType());
	}

	return false;
}

std::shared_ptr <FunctionEntity> FunctionEntity::getOverridden()
{
	return overridden.expired() ? nullptr : overridden.lock();
}

bool FunctionEntity::isConcreteOverride()
{
	return concreteOverride;
}

bool FunctionEntity::isStatic()
{
	return staticFunction;
}

const char* FunctionEntity::getTypeString()
{
	return "Function";
}

}
