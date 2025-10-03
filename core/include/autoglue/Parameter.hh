#ifndef AUTOGLUE_PARAMETER_HH
#define AUTOGLUE_PARAMETER_HH

#include <autoglue/Node.hh>

namespace ag
{

class Parameter : public Node
{
public:
	Parameter(std::wstring&& name)
		: Node(std::move(name), Type::Parameter)
	{
	}
};


}

#endif
