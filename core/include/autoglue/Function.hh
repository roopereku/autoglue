#ifndef AUTOGLUE_FUNCTION_HH
#define AUTOGLUE_FUNCTION_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>

namespace ag
{

class Function : public Node
{
public:
	// TODO:
	// Function is added -> Function is given parameters -> Function is resolved through name and signature.
	// Parameters can be matched through some virtual name getter where the given parameters are tokenized
	// When the parameters are added first, the final name will be available when the next function will be added.
	// NOTE: The return type might need to be included in the detailed name in functions without parameters
	//
	// foo(int)
	// foo(ref int)
	// foo(ref int, string)

	Function(std::wstring&& name) :
		Node(std::move(name), Type::Function, parameters),
		parameters(NodeStorage::withTypes({ Type::Parameter }))
	{
	}

	NodeStorage parameters;
};


}

#endif
