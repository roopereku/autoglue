#ifndef AUTOGLUE_TYPE_USAGE_HH
#define AUTOGLUE_TYPE_USAGE_HH

#include <autoglue/TypeDefinition.hh>

#include <memory>

namespace ag
{

class Class;
class Enum;

class TypeUsage
{
public:
	TypeUsage();

	TypeUsage(std::shared_ptr <Class> node);
	TypeUsage(std::shared_ptr <Enum> node);

private:
	TypeDefinition& mUsedType;
};

}

#endif
