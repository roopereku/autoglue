#include <gen/java/BindingGenerator.hh>
#include <gen/ClassEntity.hh>
#include <gen/ScopeEntity.hh>

#include <algorithm>
#include <filesystem>
#include <cassert>

namespace gen::java
{

BindingGenerator::BindingGenerator(Backend& backend)
	: gen::BindingGenerator(backend)
{
	// Create a directory to put the java classes in.
	std::filesystem::remove_all("com");
	std::filesystem::create_directory("com");

	package.emplace("com");
}

void BindingGenerator::generateClassBeginning(ClassEntity& entity)
{
	// Since each top level class goes to its own file, open a new file.
	if(getClassDepth() == 0)
	{
		// Get the package path as a directory hierarchy.
		auto packagePath = package.top();
		std::replace(packagePath.begin(), packagePath.end(), '.', '/');

		assert(!file.is_open());
		file.open(packagePath + "/" + entity.getName() + ".java");

		file << "package " + package.top() + ";\n";
	}

	// TODO: Use protected if necessary.
	// TODO: Set inheritance.
	file << "public class " << entity.getName() << " {\n";
}

void BindingGenerator::generateClassEnding(ClassEntity& entity)
{
	file << "}\n";

	// Since each top level class goes to its own file,
	// close the current file if a top level class is being ended.
	if(getClassDepth() == 0)
	{
		assert(file.is_open());
		file.close();
	}
}

void BindingGenerator::generateEnumBeginning(EnumEntity& entity)
{
}

void BindingGenerator::generateEnumEnding(EnumEntity& entity)
{
}

void BindingGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
}

void BindingGenerator::generateFunction(FunctionEntity& entity)
{
}

void BindingGenerator::generateParameter(ParameterEntity& entity)
{
}

void BindingGenerator::generateNamedScopeBeginning(ScopeEntity& entity)
{
	package.emplace(package.top() + '.' + entity.getName());

	// Get the package path as a directory hierarchy.
	auto packagePath = package.top();
	std::replace(packagePath.begin(), packagePath.end(), '.', '/');

	// If the package directory doesn't exist, create it.
	if(!std::filesystem::is_directory(packagePath))
	{
		std::filesystem::create_directory(packagePath);
	}
}

void BindingGenerator::generateNamedScopeEnding(ScopeEntity& entity)
{
	package.pop();
}

}
