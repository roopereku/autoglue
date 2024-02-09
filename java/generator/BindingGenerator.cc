#include <gen/java/BindingGenerator.hh>
#include <gen/TypeReferenceEntity.hh>
#include <gen/FunctionEntity.hh>
#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>
#include <gen/EnumEntryEntity.hh>
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
	file << "public class " << entity.getName() << ' ';
	entity.generateBaseClasses(*this);
	file << "\n{\n";

	// Store a pointer to the "this" handle.
	// TODO: Only do this if no inheritance is present.
	file << "protected long thisHandle;\n\n";

	// TODO: Only do this if no inheritance is present.
	// Define a public getter for the object handle.
	file << "public long getThisHandle() {\n";
	file << "return thisHandle;\n}\n\n";
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
	file << "enum " << entity.getName() << " {\n";
}

void BindingGenerator::generateEnumEnding(EnumEntity& entity)
{
	// Add a getter for the integer value.
	file << "int getValue() {\nreturn mValue;\n}\n\n";

	// Generate a constructor for the enum that takes an integer.
	file << entity.getName() << "(int value) {\nmValue = value;\n}\n\n";
	file << "private int mValue;\n}\n\n";
}

void BindingGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
	// Generate enum values.
	file << entity.getName() << '(' << entity.getValue() << ')'
		<< (entity.isLast() ? ";\n" : ",\n");
}

void BindingGenerator::generateFunction(FunctionEntity& entity)
{
	// The native method name is the function name capitalized prefixed with "n".
	std::string nativeName = 'n' + entity.getName();
	nativeName[1] = toupper(nativeName[1]);

	// Declare a native method.
	file << "private native ";

	// Constructors return pointers.
	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		file << "long ";
	}

	else
	{
		inNativeDeclaration = true;
		entity.generateReturnType(*this);
		inNativeDeclaration = false;
	}

	file << nativeName << "(";

	// If "this handle" is needed, add a parameter for it.
	if(entity.needsThisHandle())
	{
		file << "long thisHandle";

		// If there are more arguments, add a comma.
		if(entity.getParameterCount())
		{
			file << ", ";
		}
	}

	inNativeDeclaration = true;
	entity.generateParameters(*this);
	inNativeDeclaration = false;
	file << ");\n\n";

	// Write the method signature.
	file << "public ";

	if(entity.getType() != FunctionEntity::Type::Constructor)
	{
		entity.generateReturnType(*this);
	}

	// TODO: Handle return value of destructor?

	file << entity.getName() << '(';
	entity.generateParameters(*this);
	file << ") {\n";

	if(entity.returnsValue())
	{
		file << "return ";
	}

	file << nativeName << "(";

	// If "this handle" is needed, pass it.
	if(entity.needsThisHandle())
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

	// JNI is written next.
	inJni = true;
	auto bridgeName = entity.getHierarchy();

	// Locate the external bridge function.
	useBridgeFormat = true;
	jni << "extern \"C\" ";

	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		jni << "void*";
	}

	else
	{
		entity.generateReturnType(*this);
	}

	jni << ' ' << bridgeName << "(";
	entity.generateParameters(*this);
	useBridgeFormat = false;
	jni << ");\n";

	// Declare the function in JNI.
	jni << "extern \"C\" JNIEXPORT ";
	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		jni << "jlong ";
	}

	else
	{
		entity.generateReturnType(*this);
	}

	jni << "JNICALL ";

	// Declare the JNI function with the appropriate parameters.
	jni << "Java_com_" <<  entity.getParent().getHierarchy() << '_' << nativeName << "(JNIEnv*, ";
	jni << (entity.needsThisHandle() ? "jobject" : "jclass");

	// If "this handle" is needed, add a parameter for it.
	if(entity.needsThisHandle())
	{
		jni << ", jlong thisHandle";
	}

	// If there are more arguments, add a comma.
	if(entity.getParameterCount())
	{
		jni << ", ";
	}

	entity.generateParameters(*this);
	jni << ")\n{\n";

	if(entity.returnsValue())
	{
		jni << "return ";
	}

	jni << bridgeName << '(';

	// If the "this handle" is needed, pass it.
	if(entity.needsThisHandle())
	{
		jni << "reinterpret_cast <void*> (thisHandle)";

		// If there are more arguments, add a comma.
		if(entity.getParameterCount())
		{
			jni << ", ";
		}
	}

	// Convert the JNI parameters to the appropriate format.
	useBridgeFormat = true;
	onlyParameterNames = true;
	entity.generateParameters(*this);
	onlyParameterNames = false;
	useBridgeFormat = false;

	jni << ");\n}\n\n";
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
			if(entity.isEnum())
			{
				// Pass the integer value of an enum to JNI.
				file << entity.getName() << ".getValue()";
			}

			else if(entity.isClass())
			{
				// Cast the given class object to the parameter type and give the pointer to JNI.
				file << entity.getName() << ".cast" << entity.getReferred().getName() << "()";
			}
		}

		else
		{
			// When passing parameters to the bridge function some special handling might be needed.
			if(useBridgeFormat)
			{
				// Cast the this handle of class types to an abstract pointer type.
				if(entity.isClass())
				{
					target << "reinterpret_cast <void*> (" << entity.getName() << ')';
					return;
				}
			}

			target << entity.getName();
		}
	}

	else
	{
		// Bridge parameter types needs special handling.
		if(useBridgeFormat)
		{
			if(entity.isEnum())
			{
				jni << "int";
			}

			else if(entity.isClass())
			{
				jni << "void*";
			}

			else
			{
				target << entity.getReferred().getName();
			}

			return;
		}

		// Enum parameters require special handling in JNI and native method declarations.
		if(entity.isEnum())
		{
			if(inJni)
			{
				jni << "jint " << entity.getName();
				return;
			}

			else if(inNativeDeclaration)
			{
				file << "int " << entity.getName();
				return;
			}
		}

		// Class types are takes as a long in the JNI.
		else if(inJni && entity.isClass())
		{
			target << "jlong " << entity.getName();
			return;
		}

		target << entity.getReferred().getName() << ' ' << entity.getName();
	}
}

void BindingGenerator::generateBaseClass(ClassEntity& entity, size_t index)
{
	if(index == 0)
	{
		file << "extends " << entity.getName();
	}

	else
	{
		// TODO: Maybe every base class should have "implements" in the case of multiple inheritance.
		file << "implements " << entity.getName();
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
	if(inJni)
	{
		jni << ", ";
	}

	else
	{
		file << ", ";
	}
}

}
