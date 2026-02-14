#ifndef AUTOGLUE_GENERATOR_HH
#define AUTOGLUE_GENERATOR_HH

#include <autoglue/Node.hh>

namespace ag
{

class Class;
class Enum;
class Function;
class Scope;
class Field;

/// Generator is the base class for all implementations of generating
/// a specific output from a simplified hierarchy.
class Generator
{
public:
	void generate(std::shared_ptr <Node> node);

protected:
	virtual void generateClass(std::shared_ptr <Class> node) = 0;
	virtual void generateEnum(std::shared_ptr <Enum> node) = 0;
	virtual void generateFunction(std::shared_ptr <Function> node) = 0;
	virtual void generateScope(std::shared_ptr <Scope> node) = 0;
	virtual void generateField(std::shared_ptr <Field> node) = 0;
};

}

#endif
