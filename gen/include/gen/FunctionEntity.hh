#ifndef GEN_FUNCTION_ENTITY_HH
#define GEN_FUNCTION_ENTITY_HH

#include <gen/Entity.hh>
#include <gen/ClassEntity.hh>

namespace gen
{

class FunctionEntity : public Entity
{
public:
	struct Parameter
	{
		Parameter(std::string&& name, std::shared_ptr <ClassEntity> type)
			: name(std::move(name)), type(type)
		{
		}

		std::string name;
		std::shared_ptr <ClassEntity> type;
	};

	FunctionEntity(std::string_view name) : Entity(name)
	{
	}

	void addParameter(Parameter&& parameter)
	{
		parameters.emplace_back(std::move(parameter));
	}

	const char* getTypeString() override
	{
		return "Function";
	}

private:
	std::vector <Parameter> parameters;
};

}

#endif
