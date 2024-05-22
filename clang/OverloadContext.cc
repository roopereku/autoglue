#include <autoglue/clang/OverloadContext.hh>

#include <clang/AST/DeclCXX.h>

namespace ag::clang
{

OverloadContext::OverloadContext(::clang::FunctionDecl* decl,
								std::weak_ptr <FunctionGroupEntity> privateOverrides)
	: EntityContext(Type::Overload), privateOverrides(privateOverrides)
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

bool OverloadContext::isPrivateOverride()
{
	return !privateOverrides.expired();
}

std::shared_ptr <FunctionGroupEntity> OverloadContext::getOverriddenInterface()
{
	return privateOverrides.lock();
}
}
