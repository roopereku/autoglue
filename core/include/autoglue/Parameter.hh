#ifndef AUTOGLUE_PARAMETER_HH
#define AUTOGLUE_PARAMETER_HH

#include <autoglue/Variable.hh>

namespace ag
{

class Parameter : public Variable
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Parameter;
	}

	Parameter(std::wstring&& name)
		: Variable(std::move(name), Node::Type::Parameter)
	{
	}
};


}

#endif
