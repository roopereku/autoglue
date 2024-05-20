#ifndef AUTOGLUE_CLANG_ENTITY_CONTEXT_HH
#define AUTOGLUE_CLANG_ENTITY_CONTEXT_HH

#include <autoglue/EntityContext.hh>

#include <string>
#include <memory>

namespace ag::clang
{

class TypeContext;
class TyperefContext;
class FunctionContext;
class OverloadContext;

class EntityContext : public ag::EntityContext, public std::enable_shared_from_this <EntityContext>
{
public:
	enum class Type
	{
		Typeref,
		Type,
		Function,
		Overload
	};

	EntityContext(Type type);

	/// Gets this Clang entity context as a type context.
	/// 
	/// \return This context as a type context.
	std::shared_ptr <TypeContext> getTypeContext();

	/// Gets this Clang entity context as a typeref context.
	/// 
	/// \return This context as a typeref context.
	std::shared_ptr <TyperefContext> getTyperefContext();

	/// Gets this Clang entity context as a function context.
	/// 
	/// \return This context as a function context.
	std::shared_ptr <FunctionContext> getFunctionContext();

	/// Gets this Clang entity context as an overload context.
	/// 
	/// \return This context as an overload context.
	std::shared_ptr <OverloadContext> getOverloadContext();

private:
	Type type;
};

}

#endif
