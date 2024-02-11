#ifndef CLANG_GEN_ENTITY_CONTEXT_HH
#define CLANG_GEN_ENTITY_CONTEXT_HH

#include <gen/EntityContext.hh>

#include <string>

namespace gen::clang
{

class EntityContext : public gen::EntityContext
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
