#include <autoglue/Field.hh>
#include <autoglue/Class.hh>

namespace ag
{

const AbstractNode& AbstractField::getParent() const
{
	return getParentClass();
}

}
