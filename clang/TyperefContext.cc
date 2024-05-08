#include <autoglue/clang/TyperefContext.hh>

namespace ag::clang
{

TyperefContext::TyperefContext(::clang::QualType type)
	: EntityContext(Type::Typeref)
{
	if(type->isPointerType())
	{
		pointer = true;
		type = type->getPointeeType();
	}

	rvalueReference = type->isRValueReferenceType();
	type = type.getNonReferenceType();

	constType = type.isConstQualified();
	writtenType = type.getCanonicalType().getAsString();
}

bool TyperefContext::isRValueReference()
{
	return rvalueReference;
}

bool TyperefContext::isPointer()
{
	return pointer;
}

bool TyperefContext::isConst()
{
	return constType;
}

const std::string& TyperefContext::getWrittenType()
{
	return writtenType;
}

}
