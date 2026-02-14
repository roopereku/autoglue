#ifndef AUTOGLUE_CHARACTER_HH
#define AUTOGLUE_CHARACTER_HH

#include <autoglue/TypeDefinition.hh>

namespace ag
{

class Character : public TypeDefinition
{
public:
	Character()
		: TypeDefinition(Type::Character)
	{
	}

	// TODO: Specify size / width?
};

}

#endif
