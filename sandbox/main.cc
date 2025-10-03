#include <autoglue/Scope.hh>

#include <memory>

int main()
{
	auto global = std::make_shared <ag::Scope> (L"");

	global->children.add(std::make_shared <ag::Scope> (L"test"));
	auto test = global->find(L"test");

	test->as <ag::Scope> ()->children.add(std::make_shared <ag::Scope> (L"test2"));
	auto test2 = global->find(L"test.test2");

	printf("%ls\n", test2->getName().c_str());
}
