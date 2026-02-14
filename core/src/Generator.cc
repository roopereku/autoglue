#include <autoglue/Generator.hh>
#include <autoglue/Function.hh>
#include <autoglue/Class.hh>
#include <autoglue/Scope.hh>
#include <autoglue/Enum.hh>
#include <autoglue/Field.hh>

namespace ag
{

void Generator::generate(std::shared_ptr <Node> node)
{
	switch(node->getType())
	{
		case Node::Type::Scope:
		{
			generateScope(node->as <Scope> ());
			break;
		}

		case Node::Type::Class:
		{
			generateClass(node->as <Class> ());
			break;
		}

		case Node::Type::Enum:
		{
			generateEnum(node->as <Enum> ());
			break;
		}

		case Node::Type::EnumValue:
		{
			break;
		}

		case Node::Type::Function:
		{
			generateFunction(node->as <Function> ());
			break;
		}

		case Node::Type::Parameter:
		{
			break;
		}

		case Node::Type::Field:
		{
			generateField(node->as <Field> ());
			break;
		}
	}
}

}
