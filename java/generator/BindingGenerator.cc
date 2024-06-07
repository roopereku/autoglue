#include <autoglue/java/BindingGenerator.hh>
#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/FunctionEntity.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/EnumEntity.hh>
#include <autoglue/EnumEntryEntity.hh>
#include <autoglue/TypeEntity.hh>
#include <autoglue/ScopeEntity.hh>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cassert>

namespace ag::java
{

static std::string getEscapedNameJNI(std::string_view name)
{
	std::string result;

	size_t start = 0;
	size_t index = name.find('_');

	// Append "1" after underscores.
	while(index != std::string_view::npos)
	{
		result.append(name.begin() + start, name.begin() + index);
		result += "_1";

		start = index + 1;
		index = name.find('_', index + 1);
	}

	result.append(name.begin() + start, name.end());
	return result;
}

static std::string getEntityPathJNI(Entity& entity)
{
	// Once a non-class entity is found, return its path without modifications.
	if(entity.getType() != Entity::Type::Type)
	{
		return entity.getHierarchy("_");
	}

	return getEntityPathJNI(entity.getParent()) + "_" + getEscapedNameJNI(entity.getName());
}

static std::string getFunctionNameJNI(FunctionEntity& entity)
{
	return getEntityPathJNI(entity.getParent()) + "_" + getEscapedNameJNI(entity.getBridgeName(true));
}

BindingGenerator::BindingGenerator(Backend& backend, std::string_view packagePrefix)
	: ag::BindingGenerator(backend), jni("jni_glue.cpp"), packagePrefix(packagePrefix)
{
	// Create a directory to put the java classes in.
	std::filesystem::remove_all(this->packagePrefix);
	std::filesystem::create_directory(this->packagePrefix);
	package.emplace(this->packagePrefix);

	jni << "#include <jni.h>\n";

	jni << "struct JavaString\n{\npublic:\n" <<
			"JavaString(JNIEnv* env, jstring value)" <<
			" : env(env), javaString(value), cString(env->GetStringUTFChars(value, 0)) {}\n" <<
			"~JavaString() { env->ReleaseStringUTFChars(javaString, cString); }\n" <<
			"JNIEnv* env;\njstring javaString;\nconst char* cString;\n};\n";
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
	file << "public ";

	// TODO: Once ClassEntity knows whether it is an interface, handle it here.

	// Make all nested classes static.
	if(getClassDepth() > 1)
	{
		file << "static ";
	}

	if(entity.isAbstract())
	{
		file << "abstract ";
	}

	file << "class " << entity.getName() << ' ';

	entity.generateBaseTypes(*this);

	// FIXME: Until multiple inheritance works, ignore all but the first base class.
	if(entity.hasBaseTypes())
	{
		file << " */ ";
	}

	// Add the object handle if this class has no base classes.
	if(!entity.hasBaseTypes())
	{
		file << " {\n";

		// Store a pointer to the "this" object.
		file << "protected long mObjectHandle;\n\n";

		// Define a constructor that constructs an object using an existing pointer.
		file << "public " + entity.getName() + "(long objectHandle) {\n";
		file << "mObjectHandle = objectHandle;\n}\n";

		// Define a public getter for the object handle.
		file << "public long getObjectHandle() {\n";
		file << "return mObjectHandle;\n}\n\n";
	}

	// If there are base classes, add a constructor that calls super.
	else
	{
		file << "{\n";

		// Call the pointer initialization constructor of the first base class.
		file << "public " + entity.getName() + "(long objectHandle) {\n";
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

void BindingGenerator::generateEnum(EnumEntity& entity)
{
	// If no class has been opened, this is a top level enum which needs its own file.
	if(getClassDepth() == 0)
	{
		openFile(entity);
	}

	// TODO: Use the proper visibility.
	file << "public enum " << entity.getName() << " {\n";

	// Generate the enum values.
	entity.generateValues(*this);

	// Add a getter for the integer value.
	file << "public int getValue() {\nreturn mValue;\n}\n\n";

	// Add a method that converts an integer to the corresponding enum value.
	file << "public static " << entity.getName() << " fromInt(int i) {\n";
	file << "return " << entity.getName() << ".values()[i];\n}\n";

	// Generate a constructor for the enum that takes an integer.
	file << "private " << entity.getName() << "(int value) {\nmValue = value;\n}\n\n";
	file << "private int mValue;\n}\n\n";

	// Since each top level enum goes to its own file,
	// close the current file if a top level enum is being ended.
	if(getClassDepth() == 0)
	{
		assert(file.is_open());
		file.close();
	}
}

void BindingGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
	// Generate enum values.
	file << sanitizeName(entity) << '(' << entity.getValue() << ')'
		<< (entity.isLast() ? ";\n" : ",\n");
}

void BindingGenerator::generateFunction(FunctionEntity& entity)
{
	// TODO: Implement Java destructors.
	if(entity.getType() == FunctionEntity::Type::Destructor)
	{
		return;
	}

	// TODO: Generate function overloads with the appropriate names.
	if(entity.getOverloadedOperator() != FunctionEntity::OverloadedOperator::None)
	{
		return;
	}

	std::string nativeName = entity.getBridgeName(true);

	// Declare a native method.
	file << "private static native ";
	entity.generateReturnType(*this, true);
	file << nativeName << "(";

	entity.generateParameters(*this, true, true);
	file << ");\n\n";

	// Mark overriding function appropriately.
	if(entity.isOverride())
	{
		file << "@Override\n";
	}

	// Write the method signature.
	file << "public ";

	if(entity.getType() != FunctionEntity::Type::Constructor)
	{
		if(!entity.isOverridable())
		{
			file << "final ";
		}

		entity.generateReturnType(*this, false);
	}

	std::string functionName;

	// Name constructors with the class name.
	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		functionName = entity.getParent().getName();
	}

	// If this class member function isn't an override, there's a possibility of a name clash.
	else if(entity.isClassMemberFunction() && !entity.isOverride())
	{
		auto clashing = findClashing(entity, static_cast <TypeEntity&> (entity.getParent()), 0);

		if(clashing)
		{
			// TODO: Instead of just using the parent class name, the full
			// hierarchy could be used since you could have mulitple classes
			// of the same name when they are in different scopes.
			functionName = entity.getName() + entity.getParent().getName();
		}
	}

	if(functionName.empty())
	{
		functionName = sanitizeName(entity);
	}

	file << functionName << '(';
	entity.generateParameters(*this, false, false);
	file << ") {\n";

	bool closeParenthesis = entity.generateReturnStatement(*this, false);
	file << nativeName << "(";

	// Call the native method with the passed parameters.
	onlyParameterNames = true;
	entity.generateParameters(*this, false, true);
	onlyParameterNames = false;
	file << ")";

	if(closeParenthesis)
	{
		file << ")";
	}

	// Close the function.
	file << ";\n}\n\n";

	// JNI is written next.
	inJni = true;
	auto bridgeName = entity.getBridgeName();

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
	jni << "Java_" << packagePrefix << "_" << getFunctionNameJNI(entity) << "(JNIEnv* env, jclass";

	// If there are more arguments, add a comma.
	if(entity.getParameterCount(true) > 0)
	{
		jni << ", ";
	}

	entity.generateParameters(*this, true, true);
	jni << ")\n{\n";

	closeParenthesis = entity.generateReturnStatement(*this, true);
	jni << bridgeName << '(';

	onlyParameterNames = true;
	entity.generateParameters(*this, true, true);
	onlyParameterNames = false;

	jni << ')';

	if(closeParenthesis)
	{
		jni << ')';
	}

	jni << ";\n}\n\n";
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
	// Java doesn't support aliasing primitive types.
	auto finalUnderlying = entity.getUnderlying(true);
	if(finalUnderlying->getType() == TypeEntity::Type::Primitive)
	{
		return;
	}

	// Since type aliases are just simple classes, increment the class depth.
	changeClassDepth(+1);

	// If this type alias isn't nested, create a file for it.
	if(getClassDepth() == 1)
	{
		openFile(entity);
	}

	auto underlying = entity.getUnderlying();
	assert(underlying);

	file << "public ";

	// Make all nested classes static.
	if(getClassDepth() > 1)
	{
		file << "static ";
	}

	// Java has no type aliases, so create a new class that extends the aliased type.
	// TODO: For primitive types and enums do special handling.
	file << "class " << entity.getName() << " extends " << packagePrefix << '.' << underlying->getHierarchy(".") << " {\n";

	// TODO: In order to make the type alias instantiable, generate constructors from the aliased type.
	// No JNI code is required from them as they just would call super().

	file << "public " << entity.getName() << "(long objectHandle) {\n";
	file << "super(objectHandle);\n}\n";

	// Generate delegating constructors if the type alias points to a class.
	if(finalUnderlying->getType() == TypeEntity::Type::Class)
	{
		// TODO: When type extension support is generated, generate delegating
		//  constructors for abstract classes.
		auto underlyingClass = std::static_pointer_cast <ClassEntity> (finalUnderlying);
		if(!underlyingClass->isAbstract())
		{
			// Try to find the function group for constructors.
			auto result = finalUnderlying->resolve("Constructor");
			if(result && result->getType() == Entity::Type::FunctionGroup)
			{
				// Create a delegating constructor for each of the constructor overloads.
				auto& constructors = static_cast <FunctionGroupEntity&> (*result);
				for(size_t i = 0; i < constructors.getOverloadCount(); i++)
				{
					file << "public " << entity.getName() << "(";
					constructors.getOverload(i).generateParameters(*this, false, false);

					file << ") {\nsuper(";
					delegateParameters = true;
					constructors.getOverload(i).generateParameters(*this, false, false);
					delegateParameters = false;
					file << ");\n}\n";
				}
			}
		}
	}

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

bool BindingGenerator::generateBaseType(TypeEntity& entity, size_t index)
{
	if(index == 0)
	{
		file << "extends " << packagePrefix << '.' << entity.getHierarchy(".");

		// FIXME: Until multiple inheritance works, ignore all but the first base class.
		file << "/* ";
	}

	else
	{
		// TODO: Maybe every base class should have "implements" in the case of multiple inheritance.
		file << "implements " << entity.getHierarchy(".");
	}

	return true;
}

void BindingGenerator::generateNamedScope(ScopeEntity& entity)
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

	// Generate the nested entities of this named scope.
	entity.generateNested(*this);

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
		assert(entity.isPrimitive());

		// Treat object handles as opaque pointers.
		if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::ObjectHandle)
		{
			jni << "reinterpret_cast <void*> (" << entity.getName() << ')';
		}

		else if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::String)
		{
			jni << "JavaString(env, " << entity.getName() << ").cString";
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
	if(delegateParameters)
	{
		file << sanitizeName(entity);
	}

	else if(onlyParameterNames)
	{
		file << sanitizeName(entity);

		switch(entity.getType())
		{
			case TypeEntity::Type::Alias:
			{
				TypeReferenceEntity ref("", entity.getAliasType().getUnderlying(true), entity.isReference());
				generateTyperefJava(ref);

				break;
			}

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
				case PrimitiveEntity::Type::String: typeName = "String"; break;
				case PrimitiveEntity::Type::Void: typeName = "void"; break;
			}

			file << typeName << ' ' << sanitizeName(entity);
		}

		else
		{
			file << packagePrefix << '.' << entity.getReferred().getHierarchy(".") << ' ' << sanitizeName(entity);
		}
	}
}

std::string BindingGenerator::sanitizeName(Entity& entity)
{
	auto name = entity.getName();

	if(name == "interface")
	{
		name = "interface_";
	}

	else if(name == "native")
	{
		name = "native_";
	}

	// TODO: When multiple inheritance is implemented, getObjectHandle
	//  is likely to be renamed to getSomeClassName.
	else if(name == "getObjectHandle")
	{
		name = "getObjectHandle_";
	}

	return name;
}

std::shared_ptr <FunctionEntity> BindingGenerator::findClashing(FunctionEntity& entity, TypeEntity& from, int baseDepth)
{
	// If the type to look from is a class, check if it has a clashing function.
	if(from.getType() == TypeEntity::Type::Class)
	{
		// Check if the given class has an entity with same name as the function.
		auto& classEntity = static_cast <ClassEntity&> (from);

		if(baseDepth > 0)
		{
			auto sameName = classEntity.resolve(entity.getName());

			if(sameName && sameName->getType() == Entity::Type::FunctionGroup)
			{
				auto group = std::static_pointer_cast <FunctionGroupEntity> (sameName);

				// If the entity with the same name is a function group, check if it
				// has any overload with the same parameters as the given function.
				if(group)
				{
					auto result = group->findMatchingParameters(entity);
					if(result)
					{
						// If the matched function is final or has a different return type, there's a clash.
						if(!result->isOverridable() ||
							!result->getReturnType().isIdentical(entity.getReturnType()))
						{
							return result;
						}
					}
				}
			}
		}

		// If this class didn't have a clashing function, one of its bases might.
		for(size_t i = 0; i < classEntity.getBaseTypeCount(); i++)
		{
			auto& baseType = classEntity.getBaseType(i);
			auto result = findClashing(entity, baseType, baseDepth + 1);

			if(result)
			{
				return result;
			}
		}
	}

	// When a clashing function is looked for within a type alias,
	// look for it in the underlying type instead.
	else if(from.getType() == TypeEntity::Type::Alias)
	{
		auto& aliasEntity = static_cast <TypeAliasEntity&> (from);
		auto underlying = aliasEntity.getUnderlying(true);

		assert(underlying);
		return findClashing(entity, *underlying, baseDepth);
	}

	return nullptr;
}

bool BindingGenerator::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
	if(inJni)
	{
		assert(entity.isPrimitive());
		jni << "return ";

		if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::ObjectHandle)
		{
			jni << "reinterpret_cast <jlong> (";
			return true;
		}

		else if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::String)
		{
			jni << "env->NewStringUTF(";
			return true;
		}
	}

	else
	{
		// Constructors don't return, but instead call the constructor taking an object handle.
		if(target.getType() == FunctionEntity::Type::Constructor)
		{
			file << "this(";
			return true;
		}

		switch(entity.getType())
		{
			case TypeEntity::Type::Alias:
			{
				// Because a type alias can point to any type, recursively call this
				// function with the underlying type.
				TypeReferenceEntity ref("", entity.getAliasType().getUnderlying(true), entity.isReference());
				return generateReturnStatement(ref, target);
			}

			case TypeEntity::Type::Class:
			{
				// TODO: Until type extension support is generated, don't instantiate abstract classes.
				if(entity.getClassType().isAbstract())
				{
					file << "return null; //";
					return false;
				}

				// For a class return value, instantiate new Java objects holding the resulting pointer.
				file << "return new " << packagePrefix << '.' <<
						target.getReturnType().getReferred().getHierarchy(".") << '(';

				return true;
			}

			case TypeEntity::Type::Enum:
			{
				// In order to convert integers to enum types, use the fromInt method.
				file << "return " << packagePrefix << '.' <<
						target.getReturnType().getReferred().getHierarchy(".") << ".fromInt(";
				return true;
			}

			case TypeEntity::Type::Primitive:
			{
				file << "return ";
				break;
			}
		}
	}

	return false;
}

}
