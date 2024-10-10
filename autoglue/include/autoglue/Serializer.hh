#ifndef AUTOGLUE_SERIALIZER_HH
#define AUTOGLUE_SERIALIZER_HH

#include <autoglue/BindingGenerator.hh>

namespace ag
{

class Serializer : public BindingGenerator
{
protected:
	Serializer(Backend& backend);
};

}

#endif
