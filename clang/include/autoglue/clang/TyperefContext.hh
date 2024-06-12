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
	TyperefContext(::clang::QualType type, const ::clang::ASTContext& ctx);

	bool isRValueReference();
	bool isPointer();
	bool isConst();
	bool isTypeTriviallyCopyable();

	const std::string& getWrittenType();
	const std::string& getOriginalType();

private:
	std::string writtenType;
	std::string originalType;

	bool triviallyCopyable = false;
	bool rvalueReference = false;
	bool pointer = false;
	bool constType = false;
};

}

#endif
