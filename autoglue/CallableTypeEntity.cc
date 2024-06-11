#include <autoglue/CallableTypeEntity.hh>
#include <autoglue/TypeReferenceEntity.hh>

#include <cassert>

namespace ag
{

CallableTypeEntity::CallableTypeEntity(std::shared_ptr <TypeReferenceEntity>&& returnType)
	: TypeEntity("", Type::Callable), returnType(std::move(returnType))
{
	assert(this->returnType);
	assert(this->returnType->getReferredPtr());

	fullName = "Callable_" + this->returnType->getReferred().getName();
}

void CallableTypeEntity::addParameter(std::shared_ptr <TypeReferenceEntity>&& param)
{
	assert(param);
	fullName += "_" + param->getReferred().getName();

	addChild(std::move(param));
}

const std::string& CallableTypeEntity::getName() const
{
	return fullName;
}

const char* CallableTypeEntity::getTypeString()
{
	return "Callable type";
}

void CallableTypeEntity::onGenerate(BindingGenerator& generator)
{
	// TODO: Since some languages might want to export some interface
	// that can be used to utilize a callable type entity when such
	// is used by the language bindings, something like
	// generator.generateCallableType should be called here.
}

}
