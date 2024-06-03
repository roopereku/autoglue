#include <autoglue/csharp/BindingGenerator.hh>
#include <autoglue/csharp/ClassContext.hh>

#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/TypeAliasEntity.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/ScopeEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/FunctionEntity.hh>

#include <string_view>
#include <filesystem>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <array>

namespace ag::csharp
{

std::shared_ptr <EntityContext> getCSharpContext(Entity& entity)
{
	if(entity.getContext())
	{
		return std::static_pointer_cast <EntityContext> (entity.getContext());
	}

	return nullptr;
}

void makeHierarchyInterface(TypeEntity& entity)
{
	if(entity.getType() == TypeEntity::Type::Class)
	{
		auto& classEntity = static_cast <ClassEntity&> (entity);

		if(!entity.getContext())
		{
			classEntity.initializeContext(std::make_shared <ClassContext> ());
		}

		auto ctx = getCSharpContext(classEntity);
		assert(ctx);
		ctx->getClass().setInterface();

		for(size_t i = 0; i < classEntity.getBaseTypeCount(); i++)
		{
			makeHierarchyInterface(classEntity.getBaseType(i));
		}
	}

	else if(entity.getType() == TypeEntity::Type::Alias)
	{
		auto underlying = static_cast <TypeAliasEntity&> (entity).getUnderlying();
		assert(underlying);

		makeHierarchyInterface(*underlying);
	}

	else
	{
		assert(false);
	}
}

bool isOverloadProtected(FunctionEntity& entity)
{
	if(entity.isClassMemberFunction())
	{
		auto ctx = getCSharpContext(entity.getParent());
		if(ctx && ctx->getClass().isInterface())
		{
			return false;
		}

		auto overridden = entity.getOverridden();

		if(overridden)
		{
			return isOverloadProtected(*overridden);
		}
	}

	return entity.isProtected();
}

static void sanitizeName(std::string& str)
{
	static std::array <std::string_view, 3> words
	{
		"string",
		"base",
		"interface",
	};

	// Capitalize each occurence of a reserved word.
	for(auto word : words)
	{
		size_t at = str.find(word);
		while(at != std::string::npos)
		{
			str[at] = toupper(str[at]);
			at = str.find(word, at + 1);
		}
	}
}

std::string getTypeLocation(TypeEntity& entity)
{
	// Return the location of the underlying type for type aliases.
	// TODO: Return the location of a type alias when they are generated.
	if(entity.getType() == TypeEntity::Type::Alias)
	{
		return getTypeLocation(*static_cast <TypeAliasEntity&> (entity).getUnderlying());
	}

	std::string location("gencs." + entity.getHierarchy("."));
	sanitizeName(location);

	return location;
}

static std::string sanitizeName(Entity& entity)
{
	std::string name(entity.getName());
	sanitizeName(name);

	return name;
}


BindingGenerator::BindingGenerator(ag::Backend& backend, std::string_view libName)
	: ag::BindingGenerator(backend), libName(libName)
{
	std::filesystem::remove_all("gencs");
	std::filesystem::create_directory("gencs");

	namespaces.emplace("gencs");
}

void BindingGenerator::generateClass(ClassEntity& entity)
{
	// Generate a file for each top level class.
	if(getClassDepth() == 1)
	{
		openFile(entity);
	}

	file << "public ";

	auto ctx = getCSharpContext(entity);
	bool isInterface = ctx && ctx->getClass().isInterface();

	if(isInterface)
	{
		file << "interface " << sanitizeName(entity);
	}

	else
	{
		if(entity.isAbstract())
		{
			file << "abstract ";
		}

		file << "class " << sanitizeName(entity);
	}

	entity.generateBaseTypes(*this);
	file << "\n{\n";

	// Generate support code for the object handle in non-interface classes.
	if(!isInterface)
	{
		// Store the object handle when there are no base classes.
		if(!entity.hasBaseTypes())
		{
			file << "protected IntPtr " << "mObjectHandle" << ";\n";

			file << "public static IntPtr AG_getObjectHandle(" << getTypeLocation(entity) << " obj)" <<
					"\n{\nreturn obj.mObjectHandle;\n}\n";
		}

		// Define a constructor for object handle initialization.
		file << "public " << sanitizeName(entity) << "(IntPtr objectHandle)";

		// Call the base type constructor.
		if(entity.hasBaseTypes())
		{
			file << " : base(objectHandle)\n{\n";
		}

		else
		{
			// If there are no base types, initialize the object handle.
			file << "\n{\nmObjectHandle = objectHandle;\n";
		}

		file << "}\n";
	}

	else
	{
	}

	if(!isInterface)
	{
		entity.generateInterceptionFunctions(*this);
		entity.generateInterceptionContext(*this);
	}

	entity.generateNested(*this);

	if(!isInterface)
	{
		entity.generateConcreteType(*this);
	}

	file << "}\n";

	// Close the opened file if this is a top level class.
	if(getClassDepth() == 1)
	{
		assert(file.is_open());
		file.close();
	}
}

void BindingGenerator::generateEnum(EnumEntity& entity)
{
	// If no class is active, generate an enum to its own file.
	if(getClassDepth() == 0)
	{
		openFile(entity);
	}

	file << "public ";

	// If this hides another entity in a base class, mark it as new.
	if(hidesEntity(entity, entity.getParent()))
	{
		file << "new ";
	}

	file << "enum " << sanitizeName(entity);

	// TODO: Implement other enum formats.
	assert(entity.getFormat() == EnumEntity::Format::Integer);

	file << " : int";
	file << "\n{\n";

	entity.generateValues(*this);

	file << "}\n";

	// If a file was opened, close it.
	if(getClassDepth() == 0)
	{
		assert(file.is_open());
		file.close();
	}
}

void BindingGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
	file << sanitizeName(entity) << " = " << entity.getValue();

	if(!entity.isLast())
	{
		file << ",";
	}

	file << '\n';
}

void BindingGenerator::generateFunction(FunctionEntity& entity)
{
	if(entity.getOverloadedOperator() != FunctionEntity::OverloadedOperator::None)
	{
		return;
	}

	if(entity.getType() == FunctionEntity::Type::Destructor)
	{
		return;
	}

	bool parentIsInterface = false;

	if(entity.isClassMemberFunction())
	{
		assert(entity.getParent().getType() == Entity::Type::Type);

		auto parentCtx = getCSharpContext(static_cast <TypeEntity&> (entity.getParent()));
		parentIsInterface = parentCtx && parentCtx->getClass().isInterface();
	}

	// TODO: Allow constructors for the instantiable type.
	// Don't generate constructors inside interfaces.
	if(entity.getType() == FunctionEntity::Type::Constructor && parentIsInterface)
	{
		return;	
	}

	if(!parentIsInterface && !entity.isInterface())
	{
		file << "[DllImport(\"" << libName << "\", CallingConvention = CallingConvention.Cdecl)]\n";
		file << "private static extern ";

		// Represent strings as IntPtr when returning from externed functions.
		convertStringType = true;
		entity.generateReturnType(*this, true);
		convertStringType = false;

		file << entity.getBridgeName() << '(';
		entity.generateParameters(*this, true, true);
		file << ");\n";
	}

	file << (isOverloadProtected(entity) ? "protected " : "public ");

	if(!parentIsInterface)
	{
		bool markAsOverride = false;

		if(entity.isOverride())
		{
			auto overridden = entity.getOverridden();
			assert(overridden);

			auto parentCtx = getCSharpContext(overridden->getParent());
			markAsOverride = !parentCtx || !parentCtx->getClass().isInterface();
		}

		// If this function hides another defined in a base class, mark it as new.
		if(hidesEntity(entity, entity.getParent()))
		{
			file << "new ";
		}

		if(entity.isInterface())
		{
			file << "abstract ";
		}

		else
		{
			if(entity.isOverridable())
			{
				// Overrides cannot be marked virtual.
				if(!markAsOverride)
				{
					file << "virtual ";
				}
			}

			// TODO: Add sealed somewhere here.
		}

		if(markAsOverride)
		{
			file << "override ";
		}
	}

	if(entity.getType() == FunctionEntity::Type::Constructor)
	{
		file << sanitizeName(entity.getParent()) << '(';
	}

	else
	{
		entity.generateReturnType(*this, false);
		file << sanitizeName(entity) << '(';
	}

	entity.generateParameters(*this, false, false);
	file << ')';

	if(entity.isInterface())
	{
		file << ";\n";
		return;
	}

	if(entity.getType() != FunctionEntity::Type::Constructor)
	{
		file << "\n{\n";
	}

	if(!parentIsInterface)
	{
		bool closeParenthesis = entity.generateReturnStatement(*this, false);

		// The bridge function for a constructor was already generated.
		if(entity.getType() != FunctionEntity::Type::Constructor)
		{
			entity.generateBridgeCall(*this);

			if(closeParenthesis)
			{
				file << ')';
			}

			file << ";\n";
		}
	}

	file << "}\n";
}

void BindingGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		// If a parameter pass is being generated for an interception function,
		// follow the same logic as when returning values. This works because an
		// interception function accepts POD types and converts them to C# types,
		// much like when returning values.
		if(inIntercept)
		{
			bool closeParenthesis = generateBridgeToCSharp(entity);
			file << sanitizeName(entity);

			if(closeParenthesis)
			{
				file << ')';
			}
		}

		else
		{
			bool closeParenthesis = generateCSharpToBridge(entity);
			file << sanitizeName(entity);

			if(closeParenthesis)
			{
				file << ')';
			}
		}
	}

	else
	{
		if(entity.isPrimitive())
		{
			const char* primitive = "";

			switch(entity.getPrimitiveType().getType())
			{
				case PrimitiveEntity::Type::Boolean: primitive = "bool"; break;
				case PrimitiveEntity::Type::Character: primitive = "char"; break;
				case PrimitiveEntity::Type::Integer: primitive = "int"; break;
				case PrimitiveEntity::Type::Float: primitive = "float"; break;
				case PrimitiveEntity::Type::Double: primitive = "double"; break;
				case PrimitiveEntity::Type::String: primitive = convertStringType ? "IntPtr" : "string"; break;
				case PrimitiveEntity::Type::Void: primitive = "void"; break;
				case PrimitiveEntity::Type::ObjectHandle: primitive = "IntPtr"; break;
			}

			file << primitive << ' ' << sanitizeName(entity);
		}

		else
		{
			auto location = getTypeLocation(entity.getReferred());
			sanitizeName(location);

			file << location << ' ' << sanitizeName(entity);
		}
	}
}

void BindingGenerator::generateTypeAlias(TypeAliasEntity& entity)
{
	// TODO: Implement type aliases with class inheritance?

	//if(getClassDepth() == 0)
	//{
	//	openFile(entity);
	//}

	//else
	//{
	//	return;
	//}

	//auto underlying = entity.getUnderlying()->getHierarchy(".");
	//sanitizeName(underlying);

	//file << "using " << sanitizeName(entity) << " = " << underlying << ";\n";
	////file << "public class " << sanitizeName(entity) << "{}\n";

	//if(getClassDepth() == 0)
	//{
	//	assert(file.is_open());
	//	file.close();
	//}
}

void BindingGenerator::generateBaseType(TypeEntity& entity, size_t index)
{
	if(index == 0)
	{
		file << " : ";
	}

	file << getTypeLocation(entity);
}

void BindingGenerator::generateNamedScope(ScopeEntity& entity)
{
	// Add a new namespace.
	if(namespaces.empty())
	{
		namespaces.emplace(entity.getName());
	}

	else
	{
		namespaces.emplace(namespaces.top() + '.' + entity.getName());
	}

	// Get the current namespace as a directory hierarchy.
	auto directory = namespaces.top();
	std::replace(directory.begin(), directory.end(), '.', '/');

	// If the package directory doesn't exist, create it.
	if(!std::filesystem::is_directory(directory))
	{
		std::filesystem::create_directory(directory);
	}

	// Generate the nested entities of this named scope.
	entity.generateNested(*this);

	namespaces.pop();
}

void BindingGenerator::generateArgumentSeparator()
{
	file << ", ";
}

bool BindingGenerator::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
	if(inIntercept)
	{
		file << "return ";
		return generateCSharpToBridge(entity);
	}

	if(target.getType() == FunctionEntity::Type::Constructor)
	{
		file << " : this(";
		target.generateBridgeCall(*this);
		file << ")\n{\n";

		if(target.shouldPrepareClass())
		{
			file << "AG_initializeInterceptionContext();\n";
		}

		return false;
	}

	if(target.getType() == FunctionEntity::Type::MemberFunction)
	{
		file << "return ";
		return generateBridgeToCSharp(entity);
	}

	return false;
}

bool BindingGenerator::generateBridgeToCSharp(TypeReferenceEntity& entity)
{
	switch(entity.getType())
	{
		case TypeEntity::Type::Alias:
		{
			TypeReferenceEntity underlying(
				"",
				entity.getAliasType().getUnderlying(true),
				entity.isReference()
			);

			return generateBridgeToCSharp(underlying);
		}

		case TypeEntity::Type::Class:
		{
			if(entity.getClassType().isAbstract())
			{
				file << "new " << getTypeLocation(entity.getReferred()) << ".ConcreteType(";
				return true;
			}

			else
			{
				file << "new " << getTypeLocation(entity.getReferred()) << '(';
				return true;
			}
		}

		case TypeEntity::Type::Enum:
		{
			file << "(" << getTypeLocation(entity.getReferred()) << ")";
			break;
		}

		case TypeEntity::Type::Primitive:
		{
			if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::String)
			{
				// TODO: This doesn't support UTF-8?
				file << "Marshal.PtrToStringAnsi(";
				return true;
			}

			break;
		}
	}

	return false;
}

bool BindingGenerator::generateCSharpToBridge(TypeReferenceEntity& entity)
{
	switch(entity.getType())
	{
		case TypeEntity::Type::Alias:
		{
			TypeReferenceEntity underlying(
				"",
				entity.getAliasType().getUnderlying(true),
				entity.isReference()
			);

			return generateCSharpToBridge(underlying);
		}

		case TypeEntity::Type::Class:
		{
			file << getTypeLocation(entity.getReferred()) << ".AG_getObjectHandle(";
			return true;
		}

		case TypeEntity::Type::Enum:
		{
			file << "(int)";
			break;
		}

		case TypeEntity::Type::Primitive:
		{
			break;
		}
	}

	return false;
}

void BindingGenerator::generateBridgeCall(FunctionEntity& entity)
{
	file << entity.getBridgeName() << '(';
	onlyParameterNames = true;
	entity.generateParameters(*this, false, true);
	onlyParameterNames = false;
	file << ')';
}

void BindingGenerator::generateInterceptionFunction(FunctionEntity& entity, ClassEntity& parentClass)
{
	// Are we already in an interception context (The initialization function)
	if(inIntercept)
	{
		if(onlyParameterNames)
		{
            file << ", Marshal.GetFunctionPointerForDelegate(new AG_delegate_intercept_" <<
					entity.getBridgeName(true) << "(AG_intercept_" << entity.getBridgeName(true) << "))";
		}

		// The interception delegates are treated as pointers
		else
		{
			file << ", IntPtr AG_intercept_" << entity.getBridgeName(true);
		}

		return;
	}

	// TODO: Implement overload interceptors.
	if(entity.getOverloadedOperator() != FunctionEntity::OverloadedOperator::None)
	{
		return;
	}

	// TODO: Implement destructor interceptors.
	if(entity.getType() == FunctionEntity::Type::Destructor)
	{
		return;
	}

	inIntercept = true;

	file << "[UnmanagedFunctionPointer(CallingConvention.Cdecl)]\n";
    file << "private delegate ";
	entity.generateReturnType(*this, true);
	file << "AG_delegate_intercept_" << entity.getBridgeName(true) << '(';
	entity.generateParameters(*this, true, true);
	file << ");\n";

	file << "private static ";
	entity.generateReturnType(*this, true);
	file << "AG_intercept_" << entity.getBridgeName(true) << '(';

	entity.generateParameters(*this, true, true);
	file << ")\n{\n";

	bool closeParenthesis = entity.generateReturnStatement(*this, false);

	file << "((((GCHandle)" << getObjectHandleName() << ").Target) as " <<
			parentClass.getName() << ")." << entity.getName() << '(';

	onlyParameterNames = true;
	entity.generateParameters(*this, false, false);
	onlyParameterNames = false;

	if(closeParenthesis)
	{
		file << ')';
	}

	file << ");\n}\n";
	inIntercept = false;
}

void BindingGenerator::generateInterceptionContext(ClassEntity& entity)
{
	inIntercept = true;

	file << "[DllImport(\"" << libName << "\", CallingConvention = CallingConvention.Cdecl)]\n";
	file << "private static extern void " << entity.getHierarchy() << "_AG_initializeInterceptionContext(IntPtr objectHandle, IntPtr AG_foreignObject";
	entity.generateInterceptionFunctions(*this);
	file << ");\n";

	file << "private void AG_initializeInterceptionContext()\n{\n";
	file << entity.getHierarchy() << "_AG_initializeInterceptionContext(";
	file << "mObjectHandle, (IntPtr)GCHandle.Alloc(this)";

	onlyParameterNames = true;
	entity.generateInterceptionFunctions(*this);
	onlyParameterNames = false;

	file << ");\n}\n";
	inIntercept = false;
}

std::string_view BindingGenerator::getObjectHandleName()
{
	return onlyParameterNames ? "mObjectHandle" : "objectHandle";
}

void BindingGenerator::initializeGenerationContext(Entity& entity)
{
	switch(entity.getType())
	{
		case Entity::Type::Type:
		{
			switch(static_cast <TypeEntity&> (entity).getType())
			{
				case TypeEntity::Type::Class:
				{
					auto& classEntity = static_cast <ClassEntity&> (entity);

					for(size_t i = 0; i < classEntity.getBaseTypeCount(); i++)
					{
						if(i > 0)
						{
							makeHierarchyInterface(classEntity.getBaseType(i));
						}
					}

					break;
				}

				default: {}
			}

			break;
		}

		default: {}
	}
}

void BindingGenerator::openFile(TypeEntity& entity)
{
	assert(!file.is_open());

	// Get the namespace as a directory hierarchy.
	auto directory = namespaces.top();
	std::replace(directory.begin(), directory.end(), '.', '/');

	assert(!file.is_open());
	file.open(directory + "/" + entity.getName() + ".cs");

	if(entity.getType() == TypeEntity::Type::Class)
	{
		file << "using System.Runtime.InteropServices;\n";
	}

	file << "namespace " << namespaces.top() << ";\n";
}

bool BindingGenerator::hidesEntity(Entity& entity, Entity& containing)
{
	// The containing entity should only be a class.
	if(containing.getType() != Entity::Type::Type)
	{
		return false;
	}

	// If the containing type is an alias, check using the underlying type.
	// TODO: In what case an alias can hold an entity that hides something.
	auto& typeEntity = static_cast <TypeEntity&> (containing);
	if(typeEntity.getType() == TypeEntity::Type::Alias)
	{
		return hidesEntity(entity, *static_cast <TypeAliasEntity&> (containing).getUnderlying(true));
	}

	// The containing entity should only be a class.
	else if(typeEntity.getType() != TypeEntity::Type::Class)
	{
		return false;
	}

	auto& classEntity = static_cast <ClassEntity&> (containing);

	for(size_t i = 0; i < classEntity.getBaseTypeCount(); i++)
	{
		auto& baseType = classEntity.getBaseType(i);

		// If the potentially hiding entity is a function, check if any base class of its
		// has a function of the same name with the same parameters.
		if(entity.getType() == Entity::Type::Function)
		{
			auto& functionEntity = static_cast <FunctionEntity&> (entity);
			if(functionEntity.isOverride())
			{
				return false;
			}

			auto group = baseType.resolve(functionEntity.getGroup().getName());

			if(group && group->getType() == Entity::Type::FunctionGroup)
			{
				if(static_cast <FunctionGroupEntity&> (*group).findMatchingParameters(functionEntity))
				{
					return true;
				}
			}
		}

		else
		{
			auto result = baseType.resolve(entity.getName());
			if(result && result->getType() == entity.getType())
			{
				return true;
			}
		}

		// If nothing was found in this base type, check if any entity within
		// a further base type of its is being hidden.
		if(hidesEntity(entity, baseType))
		{
			return true;
		}
	}

	return false;
}

}
