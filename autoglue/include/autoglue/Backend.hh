#ifndef AUTOGLUE_BACKEND_HH
#define AUTOGLUE_BACKEND_HH

#include <autoglue/Entity.hh>
#include <autoglue/Deserializer.hh>

namespace ag
{

class Backend
{
public:
	std::shared_ptr <Entity> getRootPtr();
	Entity& getRoot();

	bool generateHierarchy();

	/// Ensures that the glue code is generated.
	void ensureGlueGenerated();

protected:
	Backend(std::shared_ptr <Entity>&& root);
	Backend(std::unique_ptr <Deserializer>&& deserializer);

	/// Called when the glue code should be generated.
	virtual void onGenerateGlue() = 0;

	virtual bool onGenerateHierarchy() = 0;

private:
	bool glueGenerated = false;
	std::shared_ptr <Entity> root;
	std::unique_ptr <Deserializer> deserializer;
};

}

#endif
