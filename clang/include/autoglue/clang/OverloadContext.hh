#ifndef AUTOGLUE_CLANG_OVERLOAD_CONTEXT_HH
#define AUTOGLUE_CLANG_OVERLOAD_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <clang/AST/Decl.h>

#include <string>

namespace ag::clang
{

class OverloadContext : public ag::clang::EntityContext
{
public:
	OverloadContext(::clang::FunctionDecl* decl);

	const std::string& getEastQualifiers();
	bool shouldInvokeFromHelper();

private:
	std::string eastQualifiers;
};

}

#endif
