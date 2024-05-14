#include <autoglue/clang/FunctionContext.hh>

#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>

namespace ag::clang
{

FunctionContext::FunctionContext(::clang::FunctionDecl* decl)
	: EntityContext(Type::Function)
{
	// Make getAsString output "bool" instead of "_Bool".
	::clang::PrintingPolicy pp(::clang::LangOptions{});
	pp.Bool = 1;

	// If the given function is a member function, initialize the self type.
	if(auto* cxxFunctionNode = ::clang::dyn_cast <::clang::CXXMethodDecl> (decl))
	{
		selfType = cxxFunctionNode->getThisType()->getPointeeType().getAsString(pp);
	}

	originalName = decl->getNameAsString();
}

std::string_view FunctionContext::getSelfType()
{
	return selfType;
}

std::string_view FunctionContext::getOriginalName()
{
	return originalName;
}

}
