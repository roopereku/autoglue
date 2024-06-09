#include <autoglue/csharp/ClassContext.hh>

#include <autoglue/TypeAliasEntity.hh>

#include <cassert>

namespace ag::csharp
{

ClassContext::ClassContext()
	: EntityContext(Type::Class)
{
}

bool ClassContext::isCompositionBaseOf(ClassEntity& derived)
{
	return compositionBaseOf.find(derived.shared_from_this()) != compositionBaseOf.end();
}

void ClassContext::setCompositionBaseOf(ClassEntity& derived)
{
	compositionBaseOf.emplace(derived.shared_from_this());
}

void ClassContext::ensureBaseGetters(TypeEntity& entity, ClassEntity& base)
{
	if(entity.getType() == TypeEntity::Type::Alias)
	{
		auto underlying = static_cast <TypeAliasEntity&> (entity).getUnderlying();
		assert(underlying);

		ensureBaseGetters(*underlying, base);
		return;
	}

	assert(entity.getType() == TypeEntity::Type::Class);
	auto& classEntity = static_cast <ClassEntity&> (entity);

	for(size_t i = 0; i < classEntity.getDerivedCount(); i++)
	{
		auto& derived = classEntity.getDerived(i);
		if(!derived.getContext())
		{
			derived.initializeContext(std::make_shared <ClassContext> ());
		}

		auto ctx = std::static_pointer_cast <csharp::EntityContext> (derived.getContext());
		ctx->getClass().baseGetters.emplace(base.shared_from_this());
	}
}

size_t ClassContext::getBaseGetterCount()
{
	return baseGetters.size();
}

std::shared_ptr <ClassEntity> ClassContext::getBaseGetter(size_t index)
{
	auto baseGetter = std::next(baseGetters.begin(), index);
	assert(!baseGetter->expired());

	return baseGetter->lock();
}
}
