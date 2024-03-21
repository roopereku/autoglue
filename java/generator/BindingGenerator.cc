#include <autoglue/java/BindingGenerator.hh>
#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/FunctionEntity.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/EnumEntity.hh>
#include <autoglue/EnumEntryEntity.hh>
#include <autoglue/TypeEntity.hh>
#include <autoglue/ScopeEntity.hh>

#include <algorithm>
#include <filesystem>
#include <cassert>

namespace ag::java
{

BindingGenerator::BindingGenerator(Backend& backend)
	: ag::BindingGenerator(backend), jni("jni_glue.cpp")
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

void BindingGenerator::generateClass(ClassEntity& entity)
{
	// Since each top level class goes to its own file, open a new file.
	if(getClassDepth() == 1)
	{
		openFile(entity);
	}

	// TODO: Use protected if necessary.
	file << "public class " << entity.getName() << ' ';

	entity.generateBaseClasses(*this);

	// Add the object handle if this class has no base classes.
	if(!entity.hasBaseClasses())
	{
		file << " {\n";

		// Store a pointer to the "this" object.
		file << "protected long mObjectHandle;\n\n";

		// Define a protected constructor for assigning the pointer to the "this" object.
		file << "protected " + entity.getName() + "(long objectHandle) {\n";
		file << "mObjectHandle = objectHandle;\n}\n";

		// Define a public getter for the object handle.
		file << "public long getObjectHandle() {\n";
		file << "return mObjectHandle;\n}\n\n";
	}

	// If there are base classes, add an additional space.
	else
	{
		file << "{\n";

		// Call the pointer initialization constructor of the first base class.
		file << "protected " + entity.getName() + "(long objectHandle) {\n";
		file << "super(objectHandle);\n}\n";
	}

	// Generate the nested entities of this class.
	entity.generateNested(*this);

	file << "}\n";

	// Since each top level class goes to its own file,
	// close the current file if a top level class is being ended.
	if(getClassDepth() == 1)
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
	entity.generateReturnType(*this, true);
	file << nativeName << "(";

	entity.generateParameters(*this, true, true);
	file << ");\n\n";

	// Write the method signature.
	file << "public ";
	entity.generateReturnType(*this, false);
	file << entity.getName() << '(';
	entity.generateParameters(*this, false, false);
	file << ") {\n";

	// For constructors, give the resulting pointer to the constructor that saves it.
	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		file << "this(";
	}

	// For non-constructor functions that return a value, add return.
	else if(entity.returnsValue())
	{
		file << "return ";
	}

	file << nativeName << "(";

	// Call the native method with the passed parameters.
	onlyParameterNames = true;
	entity.generateParameters(*this, false, true);
	onlyParameterNames = false;
	file << ")";

	// Close the deferred constructor call.
	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		file << ")";
	}

	// Close the function.
	file << ";\n}\n\n";

	// JNI is written next.
	inJni = true;
	auto bridgeName = entity.getHierarchy();

	// Locate the external bridge function.
	inExtern = true;
	jni << "extern \"C\" ";
	entity.generateReturnType(*this, true);
	jni << ' ' << bridgeName << "(";
	entity.generateParameters(*this, true, true);
	inExtern = false;
	jni << ");\n";

	// Declare the function in JNI.
	jni << "extern \"C\" JNIEXPORT ";
	entity.generateReturnType(*this, true);
	jni << "JNICALL ";

	// Declare the JNI function with the appropriate parameters.
	jni << "Java_com_" <<  entity.getParent().getHierarchy() << '_' << nativeName << "(JNIEnv*, ";
	jni << (entity.needsThisHandle() ? "jobject" : "jclass");

	// If there are more arguments, add a comma.
	if(entity.getParameterCount(true) > 0)
	{
		jni << ", ";
	}

	entity.generateParameters(*this, true, true);
	jni << ")\n{\n";

	if(entity.returnsValue())
	{
		jni << "return ";
	}

	jni << bridgeName << '(';

	onlyParameterNames = true;
	entity.generateParameters(*this, true, true);
	onlyParameterNames = false;

	jni << ");\n}\n\n";
	inJni = false;
}

void BindingGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(inJni)
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
	// Since type aliases are just simple classes, increment the class depth.
	changeClassDepth(+1);

	// If this type alias isn't nested, create a file for it.
	if(getClassDepth() == 1)
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
	if(getClassDepth() == 1)
	{
		assert(file.is_open());
		file.close();
	}

	// Since type aliases are just simple classes, decrement the class depth.
	changeClassDepth(-1);
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
				const char* typeName = "";

				switch(entity.getPrimitiveType().getType())
				{
					case PrimitiveEntity::Type::ObjectHandle: typeName = inExtern ? "void*" : "jlong"; break;
					case PrimitiveEntity::Type::Integer: typeName = inExtern ? "int" : "jint"; break;
					case PrimitiveEntity::Type::Character: typeName = inExtern ? "char" : "jchar"; break;
					case PrimitiveEntity::Type::Boolean: typeName = inExtern ? "bool" : "jboolean"; break;
					case PrimitiveEntity::Type::Float: typeName = inExtern ? "float" : "jfloat"; break;
					case PrimitiveEntity::Type::Double: typeName = inExtern ? "double" : "jdouble"; break;
					case PrimitiveEntity::Type::Void: typeName = inExtern ? "void" : "void"; break;
					case PrimitiveEntity::Type::String: typeName = inExtern ? "const char*" : "jstring"; break;
				}

				jni << typeName << ' ' << entity.getName();
				break;
			}
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
				file << ".getObjectHandle()";
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
		if(entity.getType() == TypeEntity::Type::Primitive)
		{
			const char* typeName = "";

			switch(entity.getPrimitiveType().getType())
			{
				case PrimitiveEntity::Type::ObjectHandle: typeName = "long"; break;
				case PrimitiveEntity::Type::Integer: typeName = "int"; break;
				case PrimitiveEntity::Type::Character: typeName = "char"; break;
				case PrimitiveEntity::Type::Boolean: typeName = "boolean"; break;
				case PrimitiveEntity::Type::Float: typeName = "float"; break;
				case PrimitiveEntity::Type::Double: typeName = "double"; break;
				case PrimitiveEntity::Type::Void: typeName = "void"; break;
				case PrimitiveEntity::Type::String: typeName = "string"; break;
			}

			file << typeName << ' ' << entity.getName();
		}

		else
		{
			file << entity.getReferred().getName() << ' ' << entity.getName();
		}
	}
}

}
