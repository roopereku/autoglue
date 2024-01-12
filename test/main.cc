#include <gen/FileList.hh>
#include <gen/ClassEntity.hh>

#include <gen/clang/Backend.hh>

#include <gen/generator/PythonClassGenerator.hh>

#include <cassert>

int main(int argc, char** argv)
{
	assert(argc > 1);

	gen::FileList headers(argv[1]);
	gen::clang::Backend clangBackend(headers);

	//clangBackend.exclude("std");
	//clangBackend.exclude("__gnu_cxx");

	if(!clangBackend.generateHierarchy())
	{
		return 1;
	}

	clangBackend.getRoot().resolve("gen")->list();
}
