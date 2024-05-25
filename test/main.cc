#include <autoglue/FileList.hh>
#include <autoglue/ClassEntity.hh>

#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/GlueGenerator.hh>

#include <autoglue/java/BindingGenerator.hh>

#include <autoglue/csharp/BindingGenerator.hh>

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

	clangBackend.getRoot().resolve("std.basic_string_viewCharacter_char_traitsCharacter.size")->use();
	clangBackend.getRoot().resolve("std.basic_string_viewCharacter_char_traitsCharacter.data")->use();

	//clangBackend.getRoot().resolve("ag.Entity")->list();

	// Export glue code for C++.
	ag::clang::GlueGenerator glueGen(clangBackend);
	glueGen.generateBindings();

	// Export bindings for Java.
	ag::java::BindingGenerator javaGen(clangBackend, "org");
	javaGen.generateBindings();

	// Export bindings for C#.
	ag::csharp::BindingGenerator csGen(clangBackend, "libcppglue.so");
	csGen.generateBindings();
}
