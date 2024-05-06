#ifndef AUTOGLUE_CLANG_INCLUDE_CONTEXT_HH
#define AUTOGLUE_CLANG_INCLUDE_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <string>

namespace ag::clang
{

class IncludeContext : public ag::clang::EntityContext
{
public:
	IncludeContext(std::string&& includePath)
		: EntityContext(Type::Include), includePath(std::move(includePath))
	{
	}

	const std::string& getInclude()
	{
		return includePath;
	}

private:
	std::string includePath;
};

}

#endif
