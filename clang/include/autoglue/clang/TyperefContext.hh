#ifndef AUTOGLUE_CLANG_TYPEREF_CONTEXT_HH
#define AUTOGLUE_CLANG_TYPEREF_CONTEXT_HH

#include <autoglue/clang/EntityContext.hh>

#include <string>

namespace ag::clang
{

class TyperefContext : public ag::clang::EntityContext
{
public:
	TyperefContext(std::string&& writtenType)
		: EntityContext(Type::Typeref), writtenType(std::move(writtenType))
	{
	}

	const std::string& getWrittenType()
	{
		return writtenType;
	}

private:
	std::string writtenType;
};

}

#endif
