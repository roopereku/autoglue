#include <autoglue/clang/FunctionContext.hh>

#include <clang/AST/DeclCXX.h>
#include <clang/AST/PrettyPrinter.h>

namespace ag::clang
{

FunctionContext::FunctionContext(::clang::FunctionDecl* decl)
	: EntityContext(Type::Function)
{
	// Make getAsString output "bool" instead of "_Bool" and ignore "class".
	::clang::PrintingPolicy pp(::clang::LangOptions{});
	pp.SuppressTagKeyword = 1;
	pp.Bool = 1;

	// If the given function is a member function, initialize the self type.
	if(auto* cxxFunctionNode = ::clang::dyn_cast <::clang::CXXMethodDecl> (decl))
	{
		auto type = cxxFunctionNode->getThisType()->getPointeeType();
		type = type.getUnqualifiedType();

		selfType = type.getAsString(pp);
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
