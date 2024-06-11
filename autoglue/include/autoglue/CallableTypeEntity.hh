#ifndef AUTOGLUE_CALLABLE_TYPE_ENTITY_HH
#define AUTOGLUE_CALLABLE_TYPE_ENTITY_HH

#include <autoglue/TypeEntity.hh>

namespace ag
{

class CallableTypeEntity : public TypeEntity
{
public:
	CallableTypeEntity(std::shared_ptr <TypeReferenceEntity>&& returnType);

	void addParameter(std::shared_ptr <TypeReferenceEntity>&& param);
	const std::string& getName() const override;

	const char* getTypeString() override;

private:
	void onGenerate(BindingGenerator& generator) override;

	std::string fullName;
	std::shared_ptr <TypeReferenceEntity> returnType;
};

}

#endif
