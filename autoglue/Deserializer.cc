#include <autoglue/Deserializer.hh>
#include <autoglue/ScopeEntity.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/FunctionEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>

#include <cassert>

namespace ag
{

Deserializer::Deserializer()
{
}

Deserializer::~Deserializer()
{
}

std::shared_ptr <Entity> Deserializer::addEntity(std::string_view type, ElementContext& ctx)
{
	(void)ctx;
	std::shared_ptr <Entity> result;
	bool addUnderParent = true;

	// TODO: If getTypeString was static, that could be used instead for comparisons.

	if(type == "Scope")
	{
		result = std::make_shared <ScopeEntity> (ctx.getEntityName());
	}

	else if(type == "Class")
	{
		result = std::make_shared <ClassEntity> (ctx.getEntityName());
	}

	else if(type == "FunctionGroup")
	{
		result = std::make_shared <FunctionGroupEntity> (
			ctx.getEntityName(),
			ctx.getFunctionGroupType()
		);
	}

	else if(type == "Function")
	{
		auto ret = ctx.getReturnType();
		assert(ret);

		// TODO: Use actual function modifiers.
		auto entity  = std::make_shared <FunctionEntity> (
			std::move(ret), false, false, false, false
		);

		result = entity;
		assert(ctx.getParent()->getType() == Entity::Type::FunctionGroup);
		std::static_pointer_cast <FunctionGroupEntity> (ctx.getParent())->addOverload(std::move(entity));

		addUnderParent = false;
	}

	else
	{
		printf("[ag::Deserializer::createEntity] Unimplemented '%s'\n", std::string(type).c_str());
	}

	if(result && addUnderParent)
	{
		auto copy = result;
		ctx.getParent()->addChild(std::move(copy));
	}

	return result;
}

}
