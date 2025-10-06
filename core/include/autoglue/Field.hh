#ifndef AUTOGLUE_FIELD_HH
#define AUTOGLUE_FIELD_HH

#include <autoglue/Variable.hh>

namespace ag
{

// Field defines a variable that's a member of a class.
class Field : public Variable
{
public:
	constexpr static bool matchType(Node::Type type)
	{
		return type == Node::Type::Field;
	}

	Field(std::wstring&& name)
		: Variable(std::move(name), Node::Type::Field)
	{
	}
};


}

#endif
