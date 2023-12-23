#ifndef GEN_BINDING_GENERATOR_HH
#define GEN_BINDING_GENERATOR_HH

#include <gen/ClassEntity.hh>
#include <gen/FunctionEntity.hh>

namespace gen
{

class BindingGenerator
{
public:

protected:
	virtual void generateClassBeginning(ClassEntity&) {};
	virtual void generateClassEnding(ClassEntity&) {};

	virtual void generateFunction(ClassEntity&) {};

private:
	unsigned classDepth = 0;
};

}

#endif
