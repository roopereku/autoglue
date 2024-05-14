#include <autoglue/clang/EntityContext.hh>
#include <autoglue/clang/IncludeContext.hh>
#include <autoglue/clang/TyperefContext.hh>
#include <autoglue/clang/FunctionContext.hh>

#include <cassert>

namespace ag::clang
{

EntityContext::EntityContext(EntityContext::Type type)
	: type(type)
{
}

std::shared_ptr <IncludeContext> EntityContext::getIncludeContext()
{
	assert(type == Type::Include);
	return std::static_pointer_cast <IncludeContext> (shared_from_this());
}

std::shared_ptr <TyperefContext> EntityContext::getTyperefContext()
{
	assert(type == Type::Typeref);
	return std::static_pointer_cast <TyperefContext> (shared_from_this());
}

std::shared_ptr <FunctionContext> EntityContext::getFunctionContext()
{
	assert(type == Type::Function);
	return std::static_pointer_cast <FunctionContext> (shared_from_this());
}

}
