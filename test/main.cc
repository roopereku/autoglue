#include <gen/FileList.hh>
#include <gen/ClassEntity.hh>

#include <gen/clang/Backend.hh>
#include <gen/clang/GlueGenerator.hh>

#include <gen/java/BindingGenerator.hh>

#include <cassert>

int main(int argc, char** argv)
{
	assert(argc > 2);

	gen::FileList headers(argv[1], ".hh");
	gen::clang::Backend clangBackend(headers);

	if(!clangBackend.loadCompilationDatabase(argv[2]))
	{
		return 1;
	}

	if(!clangBackend.generateHierarchy())
	{
		return 1;
	}

	clangBackend.getRoot().resolve("gen")->useAll();

	gen::clang::GlueGenerator glueGen(clangBackend);
	glueGen.generateBindings();

	gen::java::BindingGenerator javaGen(clangBackend);
	javaGen.generateBindings();

	//clangBackend.getRoot().list();
	//clangBackend.getRoot().resolve("gen")->list();
}
