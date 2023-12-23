#ifndef GEN_ENTITY_HH
#define GEN_ENTITY_HH

#include <vector>
#include <memory>
#include <string_view>

namespace gen
{

class Entity
{
public:
	void addChild(std::shared_ptr <Entity>&& child);

private:
	std::vector <std::shared_ptr <Entity>> children;
};

}

#endif
