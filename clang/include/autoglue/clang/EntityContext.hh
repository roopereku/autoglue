#ifndef AUTOGLUE_CLANG_ENTITY_CONTEXT_HH
#define AUTOGLUE_CLANG_ENTITY_CONTEXT_HH

#include <autoglue/EntityContext.hh>

#include <string>

namespace ag::clang
{

class EntityContext : public ag::EntityContext
{
public:
	EntityContext(std::string&& includePath)
		: includePath(std::move(includePath))
	{
	}

private:
	std::string includePath;
};

}

#endif
