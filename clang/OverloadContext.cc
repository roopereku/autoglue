#include <autoglue/clang/OverloadContext.hh>

#include <clang/AST/DeclCXX.h>

namespace ag::clang
{

OverloadContext::OverloadContext(::clang::FunctionDecl* decl)
	: EntityContext(Type::Overload)
{
	if(auto* cxxDecl = ::clang::dyn_cast <::clang::CXXMethodDecl> (decl))
	{
		if(cxxDecl->isConst())
		{
			eastQualifiers += "const ";
		}
	}

	if(decl->getExceptionSpecType() == ::clang::ExceptionSpecificationType::EST_BasicNoexcept)
	{
		eastQualifiers += "noexcept ";
	}
}

const std::string& OverloadContext::getEastQualifiers()
{
	return eastQualifiers;
}

}
