#ifndef AUTOGLUE_CLANG_TYPEREF_CONTEXT_HH
#define AUTOGLUE_CLANG_TYPEREF_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <clang/AST/Type.h>

#include <string>

namespace ag::clang
{

class TyperefContext : public ag::clang::EntityContext
{
public:
	TyperefContext(::clang::QualType type);

	bool isRValueReference();
	bool isPointer();
	bool isConst();

	const std::string& getWrittenType();

private:
	std::string writtenType;

	bool rvalueReference;
	bool pointer;
	bool constType;
};

}

#endif
