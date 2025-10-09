#include <autoglue/Parameter.hh>
#include <autoglue/Function.hh>

namespace ag
{

const AbstractNode& AbstractParameter::getParent() const
{
	return getParentFunction();
}

}
