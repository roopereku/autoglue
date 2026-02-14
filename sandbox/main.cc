#include <autoglue/Scope.hh>

#include <memory>

int main()
{
	auto global = std::make_shared <ag::Scope> ("");

	global->children.add(std::make_shared <ag::Scope> ("test"));
	auto test = global->find("test");

	test->as <ag::Scope> ()->children.add(std::make_shared <ag::Scope> ("test2"));
	auto test2 = global->find("test.test2");

	printf("%ls\n", test2->getName().c_str());
}
