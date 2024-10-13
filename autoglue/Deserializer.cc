#include <autoglue/Deserializer.hh>
#include <autoglue/ScopeEntity.hh>

namespace ag
{

Deserializer::Deserializer()
	: Backend(std::make_shared <ScopeEntity> ())
{
}

}
