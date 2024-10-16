#ifndef AUTOGLUE_DESERIALIZER_HH
#define AUTOGLUE_DESERIALIZER_HH

#include <autoglue/Entity.hh>

namespace ag
{

class Deserializer
{
public:
	virtual std::shared_ptr <ag::Entity> createHierarchy() = 0;

protected:
	Deserializer();
};

}

#endif
