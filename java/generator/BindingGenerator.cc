#include <gen/java/BindingGenerator.hh>
#include <gen/TypeReferenceEntity.hh>
#include <gen/FunctionEntity.hh>
#include <gen/ClassEntity.hh>
#include <gen/ScopeEntity.hh>

#include <algorithm>
#include <filesystem>
#include <cassert>

namespace gen::java
{

BindingGenerator::BindingGenerator(Backend& backend)
	: gen::BindingGenerator(backend), jni("jni_glue.cpp")
{
	// Create a directory to put the java classes in.
	std::filesystem::remove_all("com");
	std::filesystem::create_directory("com");
	package.emplace("com");

	jni << "#include <jni.h>\n";
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

	// Store a pointer to the "this" handle.
	// TODO: Only do this if no inheritance is present.
	file << "protected long thisHandle;\n\n";

	auto prefix = getHierarchyPrefix(entity);
	auto nativeName = 'n' + prefix + "_cast" + entity.getName();

	// Declare a native method for the caster function.
	file << "private native " << nativeName << "(long thisHandle);\n\n";

	// Define a public interface for the caster funciton.
	file << "public long cast" + entity.getName() << "() {\n";
	file << "return " << nativeName << "(thisHandle);\n}\n\n";

	auto packagePath = package.top();
	std::replace(packagePath.begin(), packagePath.end(), '.', '_');

	// Define a JNI bridge for the caster function.
	// TODO: Make this call a function provided by the glue code.
	jni << "extern \"C\" JNIEXPORT jlong JNICALL " << nativeName << "(JNIEnv*, jobject, jlong thisHandle)\n{\n";
	jni << "return thisHandle;\n}\n\n";
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
	// The native method name is the function name capitalized prefixed with "n".
	std::string nativeName = 'n' + entity.getName();
	nativeName[1] = toupper(nativeName[1]);

	// Declare a native method.
	file << "private native void " << nativeName << "(";
	entity.generateParameters(*this);
	file << ");\n\n";

	// Write the method signature.
	file << "public void " << entity.getName() << '(';
	entity.generateParameters(*this);
	file << ") {\n";

	file << nativeName << "(";

	// If this function is inside a class, pass "thisHandle" as the first argument.
	if(getClassDepth() > 0)
	{
		file << "thisHandle";

		// If there are more arguments, add a comma.
		if(entity.getParameterCount())
		{
			file << ", ";
		}
	}

	// Call the native method with the passed parameters.
	onlyParameterNames = true;
	entity.generateParameters(*this);
	onlyParameterNames = false;
	file << ");\n";

	// Close the function.
	file << "}\n\n";

	// Declare the function in JNI.
	jni << "extern \"C\" JNIEXPORT ";
	// TODO: Use the actual return type.
	jni << "void ";
	jni << "JNICALL ";

	auto packagePath = package.top();
	std::replace(packagePath.begin(), packagePath.end(), '.', '_');

	// Declare the JNI function with the appropriate parameters.
	// TODO: jobject -> jclass for static functions and constructors.
	jni << "Java_" << packagePath << '_' << nativeName << "(JNIEnv*, jobject, jlong thisHandle";

	inJni = true;
	if(entity.getParameterCount() > 0)
	{
		jni << ", ";
		entity.generateParameters(*this);
	}

	jni << ")\n{\n";
	jni << "}\n\n";
	inJni = false;
}

void BindingGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	auto& target = inJni ? jni : file;

	if(onlyParameterNames)
	{
		// If an argument name is used in non JNI context, pass it in the
		// appropriate way.
		if(!inJni)
		{
			auto& type = entity.getType();
			target << entity.getName() << ".cast" << type.getName() << "()";
		}

		else
		{
			target << entity.getName();
		}
	}

	else
	{
		target << entity.getType().getName() << ' ' << entity.getName();
	}
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

bool BindingGenerator::isTrivialType(ClassEntity& entity)
{
	const auto& name = entity.getName();

	return name == "string" ||
			name == "int";
}

void BindingGenerator::generateArgumentSeparator()
{
	file << ", ";
}

std::string BindingGenerator::getHierarchyPrefix(Entity& entity)
{
	if(entity.isRoot())
	{
		return entity.getName();
	}

	return getHierarchyPrefix(entity.getParent()) + '_' + entity.getName();
}

}
