#include <autoglue/clang/TyperefContext.hh>

namespace ag::clang
{

TyperefContext::TyperefContext(::clang::QualType type)
	: EntityContext(Type::Typeref)
{
	if(type->isPointerType())
	{
		type = type->getPointeeType();
	}

	rvalueReference = type->isRValueReferenceType();
	type = type.getNonReferenceType();

	writtenType = type.getCanonicalType().getAsString();
}

bool TyperefContext::isRValueReference()
{
	return rvalueReference;
}

const std::string& TyperefContext::getWrittenType()
{
	return writtenType;
}

}
