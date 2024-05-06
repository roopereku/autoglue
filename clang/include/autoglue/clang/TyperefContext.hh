#ifndef AUTOGLUE_CLANG_TYPEREF_CONTEXT_HH
#define AUTOGLUE_CLANG_TYPEREF_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <string>

namespace ag::clang
{

class TyperefContext : public ag::clang::EntityContext
{
public:
	TyperefContext() : EntityContext(Type::Typeref)
	{
	}

private:
};

}

#endif
