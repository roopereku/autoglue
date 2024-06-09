#ifndef AUTOGLUE_CSHARP_CLASS_CONTEXT_HH
#define AUTOGLUE_CSHARP_CLASS_CONTEXT_HH

#include <autoglue/csharp/EntityContext.hh>

#include <autoglue/ClassEntity.hh>

#include <memory>
#include <vector>
#include <set>

namespace ag::csharp
{

class ClassContext : public EntityContext
{
public:
	ClassContext();

	bool isCompositionBaseOf(ClassEntity& derived);

	void setCompositionBaseOf(ClassEntity& derived);

	void ensureBaseGetters(TypeEntity& entity, ClassEntity& base);

	size_t getBaseGetterCount();
	std::shared_ptr <ClassEntity> getBaseGetter(size_t index);

private:
	using PtrType = std::weak_ptr <ClassEntity>;
	std::set <PtrType, std::owner_less <PtrType>> compositionBaseOf;

	std::set <PtrType, std::owner_less <PtrType>> baseGetters;
};

}

#endif
