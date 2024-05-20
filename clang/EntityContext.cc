#include <autoglue/clang/EntityContext.hh>
#include <autoglue/clang/TypeContext.hh>
#include <autoglue/clang/TyperefContext.hh>
#include <autoglue/clang/FunctionContext.hh>
#include <autoglue/clang/OverloadContext.hh>

#include <cassert>

namespace ag::clang
{

EntityContext::EntityContext(EntityContext::Type type)
	: type(type)
{
}

std::shared_ptr <TypeContext> EntityContext::getTypeContext()
{
	assert(type == Type::Type);
	return std::static_pointer_cast <TypeContext> (shared_from_this());
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

std::shared_ptr <OverloadContext> EntityContext::getOverloadContext()
{
	assert(type == Type::Overload);
	return std::static_pointer_cast <OverloadContext> (shared_from_this());
}

}
