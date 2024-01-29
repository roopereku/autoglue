#ifndef CLANG_GEN_TYPE_CONTEXT_HH
#define CLANG_GEN_TYPE_CONTEXT_HH

#include <gen/TypeContext.hh>

#include <clang-c/Index.h>

namespace gen::clang
{

class TypeContext : public gen::TypeContext
{
public:
	TypeContext(CXType cursorType)
		: cursorType(cursorType)
	{
	}

private:
	CXType cursorType;
};

}

#endif
