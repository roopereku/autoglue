#include <autoglue/csharp/EntityContext.hh>
#include <autoglue/csharp/ClassContext.hh>

#include <cassert>

namespace ag::csharp
{

EntityContext::EntityContext(Type type)
	: type(type)
{
}

ClassContext& EntityContext::getClass()
{
	assert(type == Type::Class);
	return static_cast <ClassContext&> (*this);
}

}
