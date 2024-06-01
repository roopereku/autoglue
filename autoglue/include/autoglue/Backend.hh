#ifndef AUTOGLUE_BACKEND_HH
#define AUTOGLUE_BACKEND_HH

#include <autoglue/Entity.hh>

namespace ag
{

class Backend
{
public:
	std::shared_ptr <Entity> getRootPtr();
	Entity& getRoot();

	virtual bool generateHierarchy() = 0;

	/// Ensures that the glue code is generated.
	void ensureGlueGenerated();

protected:
	Backend(std::shared_ptr <Entity>&& root);

	/// Called when the glue code should be generated.
	virtual void generateGlue() = 0;

private:
	bool glueGenerated = false;
	std::shared_ptr <Entity> root;
};

}

#endif
