#ifndef AUTOGLUE_CLANG_TYPE_CONTEXT_HH
#define AUTOGLUE_CLANG_TYPE_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <string>

namespace ag::clang
{

class TypeContext : public ag::clang::EntityContext
{
public:
	TypeContext(std::string&& includePath, std::string&& realName)
		: EntityContext(Type::Type), includePath(std::move(includePath)), realName(std::move(realName))
	{
	}

	const std::string& getInclude()
	{
		return includePath;
	}

	const std::string& getRealName()
	{
		return realName;
	}

private:
	std::string includePath;
	std::string realName;
};

}

#endif
