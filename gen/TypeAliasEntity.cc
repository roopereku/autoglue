#include <gen/TypeAliasEntity.hh>
#include <gen/BindingGenerator.hh>

namespace gen
{

TypeAliasEntity::TypeAliasEntity(std::string_view name, std::shared_ptr <TypeEntity> underlying)
	: TypeEntity(name, Type::Alias), underlying(underlying)
{
}

std::shared_ptr <TypeEntity> TypeAliasEntity::getUnderlying()
{
	return underlying.expired() ? nullptr : underlying.lock();
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
	return "Type alias";
}

}
