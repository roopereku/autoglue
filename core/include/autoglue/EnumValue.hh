#ifndef AUTOGLUE_ENUM_VALUE_HH
#define AUTOGLUE_ENUM_VALUE_HH

#include <autoglue/Node.hh>

namespace ag
{

class EnumValue : public Node
{
public:
	EnumValue(std::wstring&& name)
		: Node(std::move(name), Type::EnumValue)
	{
	}
};


}

#endif
