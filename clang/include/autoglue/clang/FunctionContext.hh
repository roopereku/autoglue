#ifndef AUTOGLUE_CLANG_FUNCTION_CONTEXT_HH
#define AUTOGLUE_CLANG_FUNCTION_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <clang/AST/Decl.h>

#include <string_view>

namespace ag::clang
{

class FunctionContext : public ag::clang::EntityContext
{
public:
	FunctionContext(const ::clang::FunctionDecl* decl);

	std::string_view getSelfType();
	std::string_view getOriginalName();

private:
	std::string selfType;
	std::string originalName;
};

}

#endif
