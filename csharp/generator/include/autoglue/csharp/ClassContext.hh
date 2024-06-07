#ifndef AUTOGLUE_CSHARP_CLASS_CONTEXT_HH
#define AUTOGLUE_CSHARP_CLASS_CONTEXT_HH

#include <autoglue/csharp/EntityContext.hh>

#include <autoglue/ClassEntity.hh>

#include <memory>
#include <set>

namespace ag::csharp
{

class ClassContext : public EntityContext
{
public:
	ClassContext() : EntityContext(Type::Class)
	{
	}

	bool isCompositionBaseOf(ClassEntity& derived)
	{
		return compositionBaseOf.find(derived.shared_from_this()) != compositionBaseOf.end();
	}

	void setCompositionBaseOf(ClassEntity& derived)
	{
		compositionBaseOf.emplace(derived.shared_from_this());
	}

private:
	using PtrType = std::weak_ptr <ClassEntity>;
	std::set <PtrType, std::owner_less <PtrType>> compositionBaseOf;
};

}

#endif
