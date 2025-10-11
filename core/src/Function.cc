#include <autoglue/Function.hh>
#include <autoglue/Parameter.hh>
#include <autoglue/Token.hh>

#include <cassert>

namespace ag
{
Function::Function(std::wstring&& name) :
	Node(std::move(name), Node::Type::Function, parameters),
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

	// Remove parentheses.
	parameterBlock = parameterBlock.substr(1, parameterBlock.size() - 2);

	for (auto& node : parameters)
	{
		// No more parameters to compare. Mismatch in given signature.
		if (parameterBlock.empty())
		{
			return false;
		}

		// Parameter types are separated by commas.
		auto token = extractUntil(parameterBlock, ',');
		if (token.empty())
		{
			token = trim(parameterBlock);
			parameterBlock = L"";
		}

		auto param = node->as <Parameter> ();
		assert(param);

		// If the type usage held in the current token doesn't match, the given signature doesn't match.
		if (!param->getInitializerType()->matchName(token))
		{
			return false;
		}
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
