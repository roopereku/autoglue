#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/BindingGenerator.hh>

#include <cassert>

namespace ag
{

TypeAliasEntity::TypeAliasEntity(std::string_view name, std::shared_ptr <TypeEntity> underlying)
	: TypeEntity(name, Type::Alias), underlying(underlying)
{
}

std::shared_ptr <TypeEntity> TypeAliasEntity::getUnderlying(bool recursive)
{
	if(recursive)
	{
		// If the underlying type is a type alias, recursively get the underlying type of that.
		auto next = getUnderlying(false);
		if(next && next->getType() == TypeEntity::Type::Alias)
		{
			return std::static_pointer_cast <TypeAliasEntity> (next)->getUnderlying(true);
		}
	}

	return underlying.expired() ? nullptr : underlying.lock();
}

std::shared_ptr <Entity> TypeAliasEntity::resolve(std::string_view qualifiedName)
{
	auto underlying = getUnderlying();
	assert(underlying);

	return underlying->resolve(qualifiedName);
}

void TypeAliasEntity::onFirstUse()
{
	if(!underlying.expired())
	{
		underlying.lock()->use();
	}
}

void TypeAliasEntity::onGenerate(BindingGenerator& generator)
{
	generator.generateTypeAlias(*this);
}

const char* TypeAliasEntity::getTypeString()
{
	return "TypeAlias";
}

}
