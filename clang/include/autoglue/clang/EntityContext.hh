#ifndef AUTOGLUE_CLANG_ENTITY_CONTEXT_HH
#define AUTOGLUE_CLANG_ENTITY_CONTEXT_HH

#include <autoglue/EntityContext.hh>

#include <string>
#include <memory>

namespace ag::clang
{

class IncludeContext;
class TyperefContext;
class FunctionContext;

class EntityContext : public ag::EntityContext, public std::enable_shared_from_this <EntityContext>
{
public:
	enum class Type
	{
		Typeref,
		Include,
		Function
	};

	EntityContext(Type type);

	/// Gets this Clang entity context as an include context.
	/// 
	/// \return This context as an include context.
	std::shared_ptr <IncludeContext> getIncludeContext();

	/// Gets this Clang entity context as a typeref context.
	/// 
	/// \return This context as a typeref context.
	std::shared_ptr <TyperefContext> getTyperefContext();

	/// Gets this Clang entity context as a function context.
	/// 
	/// \return This context as a function context.
	std::shared_ptr <FunctionContext> getFunctionContext();

private:
	Type type;
};

}

#endif
