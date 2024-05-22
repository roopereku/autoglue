#ifndef AUTOGLUE_CLANG_OVERLOAD_CONTEXT_HH
#define AUTOGLUE_CLANG_OVERLOAD_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>
#include <autoglue/FunctionGroupEntity.hh>

#include <clang/AST/Decl.h>

#include <string>

namespace ag::clang
{

class OverloadContext : public ag::clang::EntityContext
{
public:
	OverloadContext(::clang::FunctionDecl* decl,
					std::weak_ptr <FunctionGroupEntity> privateOverrides);

	const std::string& getEastQualifiers();

	bool isPrivateOverride();
	std::shared_ptr <FunctionGroupEntity> getOverriddenInterface();

private:
	std::string eastQualifiers;
	std::weak_ptr <FunctionGroupEntity> privateOverrides;
};

}

#endif
