#include <autoglue/FileList.hh>
#include <autoglue/ClassEntity.hh>

#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/GlueGenerator.hh>

#include <autoglue/java/BindingGenerator.hh>

#include <autoglue/csharp/BindingGenerator.hh>

#include <autoglue/xml/Serializer.hh>
#include <autoglue/xml/Deserializer.hh>

#include <cassert>
#include <filesystem>

ag::clang::Backend createBackend(std::string_view compileCommands, std::string_view xmlFile)
{
	if(std::filesystem::exists(xmlFile))
	{
		printf("Constructing a backend from '%s'\n", std::string(xmlFile).c_str());
		auto deserializer = std::make_unique <ag::xml::Deserializer> (xmlFile);
		ag::clang::Backend clangBackend(std::move(deserializer));

		return clangBackend;
	}

	ag::clang::Backend clangBackend(compileCommands);
	return clangBackend;
}

int main(int argc, char** argv)
{
	assert(argc > 2);

	auto clangBackend = createBackend(argv[1], argv[2]);
	if(!clangBackend.generateHierarchy())
	{
		printf("Hierarchy generation failed\n");
		return 1;
	}

	// If autoglue is given to clangBackend, this will resolve the namespace.
	auto ns = clangBackend.getRoot().resolve("ag");

	// If something was resolved, export it.
	if(ns)
	{
		ns->useAll();
	}

	// Export bindings for Java.
	ag::java::BindingGenerator javaGen(clangBackend, "org");
	javaGen.generateBindings();

	// Export bindings for C#.
	ag::csharp::BindingGenerator csGen(clangBackend, "libcppglue.so");
	csGen.generateBindings();

	ag::xml::Serializer xmlSer(clangBackend);
	xmlSer.serialize();
}
