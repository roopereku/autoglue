#ifndef AUTOGLUE_CSHARP_ENTITY_CONTEXT_HH
#define AUTOGLUE_CSHARP_ENTITY_CONTEXT_HH

#include <autoglue/EntityContext.hh>

namespace ag::csharp
{

class ClassContext;

class EntityContext : public ag::EntityContext
{
public:
	enum class Type
	{
		Class
	};

	ClassContext& getClass();

	Type type;

protected:
	EntityContext(Type type);

private:
};

}

#endif
