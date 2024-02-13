#include <autoglue/FileList.hh>
#include <autoglue/ClassEntity.hh>

#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/GlueGenerator.hh>

#include <autoglue/java/BindingGenerator.hh>

#include <cassert>

int main(int argc, char** argv)
{
	assert(argc > 1);

	ag::clang::Backend clangBackend(argv[1]);

	if(!clangBackend.generateHierarchy())
	{
		return 1;
	}

	// If autoglue is given to clangBackend, this will resolve the namespace.
	auto ns = clangBackend.getRoot().resolve("ag");

	// If something was resolved, export it.
	if(ns)
	{
		ns->useAll();
	}

	// Export glue code for C++.
	ag::clang::GlueGenerator glueGen(clangBackend);
	glueGen.generateBindings();

	// Export bindings for Java.
	ag::java::BindingGenerator javaGen(clangBackend);
	javaGen.generateBindings();
}
