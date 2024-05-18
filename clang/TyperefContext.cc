#include <autoglue/clang/TyperefContext.hh>

#include <clang/AST/PrettyPrinter.h>

namespace ag::clang
{

TyperefContext::TyperefContext(::clang::QualType type)
	: EntityContext(Type::Typeref)
{
	// Make getAsString output "bool" instead of "_Bool" and ignore "class".
	::clang::PrintingPolicy pp(::clang::LangOptions{});
	pp.SuppressTagKeyword = 1;
	pp.Bool = 1;

	if(type->isPointerType())
	{
		pointer = true;
		type = type->getPointeeType();
	}

	rvalueReference = type->isRValueReferenceType();
	type = type.getNonReferenceType();

	constType = type.isConstQualified();
	writtenType = type.getCanonicalType().getAsString(pp);
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
