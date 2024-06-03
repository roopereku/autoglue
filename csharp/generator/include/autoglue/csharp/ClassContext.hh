#ifndef AUTOGLUE_CSHARP_CLASS_CONTEXT_HH
#define AUTOGLUE_CSHARP_CLASS_CONTEXT_HH

#include <autoglue/csharp/EntityContext.hh>

namespace ag::csharp
{

class ClassContext : public EntityContext
{
public:
	ClassContext() : EntityContext(Type::Class)
	{
	}

	bool isInterface()
	{
		return interface;
	}

	void setInterface()
	{
		interface = true;
	}

private:
	bool interface;
};

}

#endif
