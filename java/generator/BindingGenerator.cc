#include <gen/java/BindingGenerator.hh>
#include <gen/TypeReferenceEntity.hh>
#include <gen/TypeAliasEntity.hh>
#include <gen/FunctionEntity.hh>
#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>
#include <gen/EnumEntryEntity.hh>
#include <gen/TypeEntity.hh>
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

void BindingGenerator::openFile(Entity& entity)
{
	// Get the package path as a directory hierarchy.
	auto packagePath = package.top();
	std::replace(packagePath.begin(), packagePath.end(), '.', '/');

	assert(!file.is_open());
	file.open(packagePath + "/" + entity.getName() + ".java");

	file << "package " + package.top() + ";\n";
}

void BindingGenerator::generateClassBeginning(ClassEntity& entity)
{
	// Since each top level class goes to its own file, open a new file.
	if(getClassDepth() == 0)
	{
		openFile(entity);
	}

	// TODO: Use protected if necessary.
	file << "public class " << entity.getName() << ' ';

	entity.generateBaseClasses(*this);

	// Add the object handle if this class has no base classes.
	if(!entity.hasBaseClasses())
	{
		file << "{\n";

		// Store a pointer to the "this" handle.
		file << "protected long thisHandle;\n\n";

		// Define a public getter for the object handle.
		file << "public long getThisHandle() {\n";
		file << "return thisHandle;\n}\n\n";
	}

	// If there are base classes, add an additional space.
	else
	{
		file << " {\n";
	}
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
	// TODO: Use the proper visibility.
	file << "public enum " << entity.getName() << " {\n";
}

void BindingGenerator::generateEnumEnding(EnumEntity& entity)
{
	// Add a getter for the integer value.
	file << "public int getValue() {\nreturn mValue;\n}\n\n";

	// Generate a constructor for the enum that takes an integer.
	file << "private " << entity.getName() << "(int value) {\nmValue = value;\n}\n\n";
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
	inExtern = true;
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
	inExtern = false;
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

	onlyParameterNames = true;
	entity.generateParameters(*this);
	onlyParameterNames = false;

	jni << ");\n}\n\n";
	inJni = false;
}

void BindingGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(inNativeDeclaration)
	{
		generateTyperefNativeDecl(entity);
	}

	else if(inJni)
	{
		generateTyperefJNI(entity);
	}

	else
	{
		generateTyperefJava(entity);
	}
}

void BindingGenerator::generateTypeAlias(TypeAliasEntity& entity)
{
	// If this type alias isn't nested, create a file for it.
	if(getClassDepth() == 0)
	{
		openFile(entity);
	}

	auto underlying = entity.getUnderlying();
	assert(underlying);

	// Java has no type aliases, so create a new class that extends the aliased type.
	// TODO: For primitive types and enums do special handling.
	file << "public class " << entity.getName() << " extends " << underlying->getName() << " {\n";

	// TODO: In order to make the type alias instantiable, generate constructors from the aliased type.
	// No JNI code is required from them as they just would call super().

	file << "}\n";

	// If this type alias isn't nested close the created file.
	if(getClassDepth() == 0)
	{
		assert(file.is_open());
		file.close();
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

void BindingGenerator::generateTyperefJNI(TypeReferenceEntity& entity)
{
	// Function calls in the JNI are always calls to bridge functions.
	if(onlyParameterNames)
	{
		if(entity.getType() == TypeEntity::Type::Class)
		{
			jni << "reinterpret_cast <void*> (" << entity.getName() << ')';
		}

		else
		{
			jni << entity.getName();
		}
	}

	else
	{
		switch(entity.getType())
		{
			// TODO: Assuming long is only valid for aliases that refer to a class.
			case TypeEntity::Type::Alias:
			case TypeEntity::Type::Class:
			{
				jni << (inExtern ? "void*" : "jlong ") << entity.getName();
				break;
			}

			case TypeEntity::Type::Enum:
			{
				jni << "jint " << entity.getName();
				break;
			}

			case TypeEntity::Type::Primitive:
			{
				// TODO: Convert to proper JNI primitives.
				jni << entity.getReferred().getName() << ' ' << entity.getName();
				break;
			}
		}
	}
}

void BindingGenerator::generateTyperefNativeDecl(TypeReferenceEntity& entity)
{
	switch(entity.getType())
	{
		// TODO: Assuming long is only valid for aliases that refer to a class.
		case TypeEntity::Type::Alias:
		case TypeEntity::Type::Class:
		{
			file << "long " << entity.getName();
			break;
		}

		case TypeEntity::Type::Enum:
		{
			file << "int " << entity.getName();
			break;
		}

		case TypeEntity::Type::Primitive:
		{
			// TODO: Convert to proper java primitives.
			file << entity.getReferred().getName() << ' ' << entity.getName();
			break;
		}
	}
}

void BindingGenerator::generateTyperefJava(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		file << entity.getName();

		switch(entity.getType())
		{
			case TypeEntity::Type::Class:
			{
				file << ".getThisHandle()";
				break;
			}

			case TypeEntity::Type::Enum:
			{
				file << ".getValue()";
				break;
			}

			default: {}
		}
	}

	else
	{
		// TODO: Convert primitive types to proper java primitives.
		file << entity.getReferred().getName() << ' ' << entity.getName();
	}
}

}
