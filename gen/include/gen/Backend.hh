#ifndef GEN_BACKEND_HH
#define GEN_BACKEND_HH

#include <gen/Entity.hh>

namespace gen
{

class Backend
{
public:
	virtual Entity& getRoot() = 0;

	virtual bool generateHierarchy() = 0;
};

}

#endif
