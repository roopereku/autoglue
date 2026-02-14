#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Token.hh>

#include <cassert>

namespace ag
{
Function::Function(std::string_view name) :
	Node(name, Node::Type::Function, parameters),
	parameters(NodeStorage::withTypes({ Type::Parameter }))
{
}

bool Function::matchName(std::string_view name) const
{
	// Check whether the portion that should represent the name matches the function name.
	if (!Node::matchName(name.substr(0, getName().size())))
	{
		return false;
	}

	// Make sure that the parameters are enclosed by parentheses.
	auto parameterBlock = name.substr(getName().size());
	if (parameterBlock.empty() || parameterBlock.front() != '(' || parameterBlock.back() != ')')
	{
		return false;
	}

	// Remove parentheses and match the parameters.
	parameterBlock = parameterBlock.substr(1, parameterBlock.size() - 2);
	return matchParameterTypes(parameterBlock);
}

size_t Function::getParameterCount() const
{
	return std::distance(parameters.begin(), parameters.end());
}

std::optional <TypeUsage> Function::getParameterType(size_t index) const
{
	auto param = getParameter(index);
	return param->getInitializerType();
}

std::shared_ptr <Parameter> Function::getParameter(size_t index) const
{
	auto it = parameters.begin() + index;
	assert(it->get());
	assert(it->get()->getType() == Node::Type::Parameter);

	return it->get()->as <Parameter> ();
}

}
