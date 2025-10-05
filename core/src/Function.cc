#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>

#include <cassert>

namespace ag
{
Function::Function(std::wstring&& name) :
	Node(std::move(name), NodeType, parameters),
	parameters(NodeStorage::withTypes({ Type::Parameter }))
{
}

bool Function::matchName(std::wstring_view name) const
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

	// TODO: Move this logic to Callable so that it can be easily reused for matching callable parameters.
	// TODO: Unnamed callables are represented as int(string, float) during matching.

	parameterBlock = parameterBlock.substr(1, parameterBlock.size() - 2);
	for (auto& param : parameters)
	{
		size_t commaAt = parameterBlock.find(',');
		std::wstring_view current;

		if (commaAt == std::wstring::npos)
		{
			// The current one is the final parameter.
			current = parameterBlock;
			parameterBlock = L"";
		}

		else
		{
			// Extract the current parameter and move on the the next one.
			current = parameterBlock.substr(0, commaAt);
			parameterBlock = parameterBlock.substr(commaAt);
		}

		// If there's no more parameters in the string, the parameter counts mismatch.
		if (current.empty())
		{
			return false;
		}

		// TODO: Match current with the type of the parameter.
		(void)param;
	}

	// If there's still something in the parameter block, the parameter counts mismatch.
	if (!parameterBlock.empty())
	{
		return false;
	}

	return true;
}

size_t Function::getParameterCount() const
{
	return std::distance(parameters.begin(), parameters.end());
}

std::optional <TypeUsage> Function::getParameterType(size_t index) const
{
	auto it = parameters.begin() + index;
	assert(it->get());
	assert(it->get()->getType() == Node::Type::Parameter);
	return it->get()->as <Parameter> ()->getInitializerType();
}

}
