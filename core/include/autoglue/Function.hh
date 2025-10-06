#ifndef AUTOGLUE_FUNCTION_HH
#define AUTOGLUE_FUNCTION_HH

#include <autoglue/Node.hh>
#include <autoglue/NodeStorage.hh>
#include <autoglue/SignatureHolder.hh>

namespace ag
{

class Function : public Node, public SignatureHolder
{
public:
	static constexpr bool matchType(Node::Type type)
	{
		return type == Node::Type::Function;
	}

	Function(std::wstring&& name);

	/// Checks if the given name matches the function name and parameters.
	/// NOTE: Only the function name is matched until the function is completed.
	///
	/// \param name The name to match against. Expected format is name(mod type1, mod type2)
	/// \return True if the name and parameters match.
	bool matchName(std::wstring_view name) const override;

	/// Enables parameter matching when calling matchName for this function.
	void enforceParameterMatching()
	{
		mMatchParameters = true;
	}

	size_t getParameterCount() const override;
	std::optional <TypeUsage> getParameterType(size_t index) const override;

	NodeStorage parameters;

private:
	/// Are parameters being matched when attempting to locate this function.
	/// This is enabled after called buildHierarchy with the function itself.
	bool mMatchParameters = false;
};


}

#endif
