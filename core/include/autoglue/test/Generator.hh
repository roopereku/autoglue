#ifndef AUTOGLUE_TEST_GENERATOR_HH
#define AUTOGLUE_TEST_GENERATOR_HH

#include <autoglue/Generator.hh>
#include <autoglue/test/NodeBase.hh>

namespace ag::test
{

class Generator : public ag::Generator
{
public:
	Generator();

	void testAgainst(std::shared_ptr <NodeBase> root) const;

protected:
	void compare(std::shared_ptr <NodeBase> generated, std::shared_ptr <NodeBase> original) const;

	void generateClass(std::shared_ptr <Class> node) override;
	void generateEnum(std::shared_ptr <Enum> node) override;
	void generateFunction(std::shared_ptr <Function> node) override;
	void generateScope(std::shared_ptr <Scope> node) override;
	void generateField(std::shared_ptr <Field> node) override;

private:
	std::shared_ptr <NodeBase> mCurrent;
};

}

#endif
