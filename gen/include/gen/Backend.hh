#ifndef GEN_BACKEND_HH
#define GEN_BACKEND_HH

#include <gen/Entity.hh>

namespace gen
{

class Backend
{
public:
	std::shared_ptr <Entity> getRootPtr()
	{
		return root;
	}

	Entity& getRoot()
	{
		return *root;
	}

	virtual bool generateHierarchy() = 0;

protected:
	Backend(std::shared_ptr <Entity>&& root)
		: root(std::move(root))
	{
	}

private:
	std::shared_ptr <Entity> root;
};

}

#endif
