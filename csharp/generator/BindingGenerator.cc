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

bool isOverloadProtected(FunctionEntity& entity)
{
	if(entity.isClassMemberFunction())
	{
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
	static std::array <std::string_view, 4> words
	{
		"string",
		"base",
		"interface",
		"object"
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
	if(entity.isAbstract())
	{
		file << "abstract ";
	}

	file << "class " << sanitizeName(entity);

	entity.generateBaseTypes(*this);
	file << "\n{\n";

	// Store the object handle when there are no base classes.
	if(!entity.hasBaseTypes())
	{
		file << "protected IntPtr " << "mObjectHandle" << ";\n";

		file << "public static IntPtr AG_getObjectHandle(" << getTypeLocation(entity) << " obj)" <<
				"\n{\nreturn obj.mObjectHandle;\n}\n";
	}

	// Define a constructor for object handle initialization.
	file << "public " << sanitizeName(entity) << "(IntPtr ObjectHandle)";

	// Call the base type constructor.
	if(entity.hasBaseTypes())
	{
		file << " : base(ObjectHandle)\n{\n";
	}

	else
	{
		// If there are no base types, initialize the object handle.
		file << "\n{\nmObjectHandle = ObjectHandle;\n";
	}

	compositionBaseTarget = entity.shared_from_this();
	inBaseInitialization = true;
	entity.generateBaseTypes(*this);
	inBaseInitialization = false;

	file << "}\n";

	entity.generateBaseTypes(*this);
	compositionBaseTarget = nullptr;

	// Write a creation function for each composition base class that
	// doesn't have such a function yet.
	if(ctx)
	{
		auto& classCtx = ctx->getClass();
		for(size_t i = 0; i < classCtx.getBaseGetterCount(); i++)
		{
			auto baseGetter = classCtx.getBaseGetter(i);
			assert(baseGetter);
			assert(baseGetter->getConcreteType());

			file << "protected override " << getTypeLocation(*baseGetter) <<
					" createBase_" << sanitizeName(*baseGetter) << "()\n{\n" <<
					"return new " << getTypeLocation(*baseGetter->getConcreteType()) <<
					"(mObjectHandle);\n}\n";
		}
	}

	entity.generateInterceptionFunctions(*this);
	entity.generateInterceptionContext(*this);

	entity.generateNested(*this);

	entity.generateConcreteType(*this);

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

bool hasCompositionBase(ClassContext& ctx, TypeEntity& entity)
{
	if(entity.getType() == TypeEntity::Type::Alias)
	{
		auto underlying = static_cast <TypeAliasEntity&> (entity).getUnderlying();
		assert(underlying);

		return hasCompositionBase(ctx, *underlying);
	}

	assert(entity.getType() == TypeEntity::Type::Class);
	auto& classEntity = static_cast <ClassEntity&> (entity);

	// If the current class has a composition base class represented 
	// by the given context, there is a composition base class.
	if(ctx.isCompositionBaseOf(classEntity))
	{
		return true;
	}

	// If no composition base class was found yet, look for such
	// in further base classes.
	for(size_t i = 0; i < classEntity.getBaseTypeCount(); i++)
	{
		if(hasCompositionBase(ctx, classEntity.getBaseType(i)))
		{
			return true;
		}
	}

	return false;
}

std::shared_ptr <FunctionEntity> getOverriddenInCompositeBase(FunctionEntity& entity)
{
	if(entity.isOverride())
	{
		auto overridden = entity.getOverridden();
		assert(overridden);

		assert(ClassEntity::matchType(entity.getParent()));
		auto& parent = static_cast <ClassEntity&> (entity.getParent());

		// If the parent class inherits a composition base class that contains
		// the overridden function, return the overridden function.
		auto overrideParentCtx = getCSharpContext(overridden->getParent());
		if(overrideParentCtx && hasCompositionBase(overrideParentCtx->getClass(), parent))
		{
			return overridden;
		}

		// If nothing was found yet, look further using the overridden function.
		return getOverriddenInCompositeBase(*overridden);
	}

	return nullptr;
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

	if(getOverriddenInCompositeBase(entity))
	{
		return;
	}

	if(!entity.isInterface())
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

	bool markAsOverride = entity.isOverride();

	if(entity.isStatic())
	{
		file << "static ";
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

	file << "}\n";
}

void BindingGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		if(delegateInterception)
		{
			file << sanitizeName(entity);
			return;
		}

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

bool BindingGenerator::generateBaseType(TypeEntity& entity, size_t index)
{
	if(compositionBaseTarget)
	{
		// Base classes after the first one are generated as member objects.
		if(index > 0)
		{
			switch(entity.getType())
			{
				case TypeEntity::Type::Alias:
				{
					auto underlying = static_cast <TypeAliasEntity&> (entity).getUnderlying(true);
					assert(underlying);

					return generateBaseType(*underlying, index);
				}

				case TypeEntity::Type::Class:
				{
					if(inBaseInitialization)
					{
						file << "base" << sanitizeName(entity) << " = createBase_" <<
								sanitizeName(entity) << "();\n";
					}

					else
					{
						file << "public readonly " << getTypeLocation(entity) <<
								' ' << "base" << sanitizeName(entity) << ";\n";

						auto classEntity = static_cast <ClassEntity&> (entity);

						// If both the derived class and the composition base class, make the
						// creator function abstract. This works because when the base class
						// is abstract and a derived class doesn't implement it, the derived
						// class is implicitly abstract as well.
						//
						// FIXME: This might not work when there are 2 abstract composition base
						// classes out of which only 1 is implemented.
						if(compositionBaseTarget->isAbstract() && classEntity.isAbstract())
						{
							file << "protected abstract " << getTypeLocation(entity) <<
									" createBase_" << sanitizeName(entity) << "();\n";
						}

						// If the base class is not abstract, provide a default implementation
						// for the getter function so that overriding it is optional.
						else
						{
							file << "protected virtual " << getTypeLocation(entity) <<
									" createBase_" << sanitizeName(entity) << "()\n{\n" <<
									"return new ";

							// If the base class has a concrete type, prefer it
							// to make virtual functions work.
							if(classEntity.getConcreteType())
							{
								file << getTypeLocation(*classEntity.getConcreteType());
							}

							else
							{
								file << getTypeLocation(entity);
							}
									
							file << "(mObjectHandle);\n}\n";
						}
					}

					break;
				}

				default:
				{
					assert(false);
				}
			}
		}
	}

	else
	{
		// Only generate the first base class when defining inheritance.
		if(index == 0)
		{
			file << " : " << getTypeLocation(entity);
		}
	}

	return false;
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
	if(delegateInterception)
	{
		file << "return ";
		return false;
	}

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

		case TypeEntity::Type::Callable:
		{
			// TODO: Something here?
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
		case TypeEntity::Type::Callable:
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
	auto overridden = getOverriddenInCompositeBase(entity);

	// Are we already in an interception context (The initialization function)
	if(inIntercept)
	{
		if(onlyParameterNames)
		{
			// FIXME: Once operator overloads are generated, pass them in correctly.
			if(entity.getOverloadedOperator() != FunctionEntity::OverloadedOperator::None)
			{
				file << ", 0";
			}

			else
			{
				// TODO: If creating a interception function for overridden, use its bridge name instead.
				file << ", Marshal.GetFunctionPointerForDelegate(new AG_delegate_intercept_" <<
						entity.getBridgeName(true) << "(AG_intercept_" << entity.getBridgeName(true) << "))";
			}
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

	file << "internal static ";
	entity.generateReturnType(*this, true);
	file << "AG_intercept_" << entity.getBridgeName(true) << '(';

	entity.generateParameters(*this, true, true);
	file << ")\n{\n";

	delegateInterception = static_cast <bool> (overridden);
	bool closeParenthesis = entity.generateReturnStatement(*this, false);

	// If the overridden function is from a composition base class, call another
	// interception function from the member representing the base class.
	if(overridden)
	{
		file << "var AG_BaseHandle = GCHandle.Alloc(" << "((((GCHandle)ObjectHandle" <<
				").Target) as " << sanitizeName(parentClass) << ").base" << sanitizeName(overridden->getParent()) << ");\n";

		assert(ClassEntity::matchType(overridden->getParent()));
		file << getTypeLocation(static_cast <ClassEntity&> (overridden->getParent())) <<
				".AG_intercept_" << overridden->getBridgeName(true) << '(';
	}

	else
	{
		file << "((((GCHandle)" << getObjectHandleName() << ").Target) as " <<
				sanitizeName(parentClass) << ")." << sanitizeName(entity) << '(';
	}

	onlyParameterNames = true;
	entity.generateParameters(*this, false, delegateInterception);
	onlyParameterNames = false;

	if(closeParenthesis)
	{
		file << ')';
	}

	file << ");\n";

	// If parameters were delegated, destroy the temporarily created GCHandle.
	if(delegateInterception)
	{
		file << "AG_BaseHandle.Free();\n";
	}

	file << "}\n";
	inIntercept = false;
	delegateInterception = false;
}

void BindingGenerator::generateInterceptionContext(ClassEntity& entity)
{
	inIntercept = true;

	file << "[DllImport(\"" << libName << "\", CallingConvention = CallingConvention.Cdecl)]\n";
	file << "private static extern void " << entity.getHierarchy() << "_AG_initializeInterceptionContext(IntPtr ObjectHandle, IntPtr AG_foreignObject";
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
	// When parameters of an interception functions are delegated to another,
	// cast the created GCHandle pointing to a composition base to a pointer.
	if(delegateInterception)
	{
		return "(IntPtr)AG_BaseHandle";
	}

	return onlyParameterNames ? "mObjectHandle" : "ObjectHandle";
}

void initializeBaseContext(ClassEntity& derived, TypeEntity& base)
{
	assert(base.getType() == TypeEntity::Type::Class);
	auto& baseClass = static_cast <ClassEntity&> (base);

	if(!base.getContext())
	{
		base.initializeContext(std::make_shared <ClassContext> ());
	}

	getCSharpContext(base)->getClass().setCompositionBaseOf(derived);

	// If the base class is abstract, ensure that getters function implementations
	// for creating an instance of this class exist for the derived types.
	if(baseClass.isAbstract())
	{
		getCSharpContext(base)->getClass().ensureBaseGetters(derived, baseClass);
	}
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
						// If a class uses multiple inheritance, treat every base class
						// that's not the first one as a composition base where it
						// will be stored as a member object.
						if(i > 0)
						{
							auto& base = classEntity.getBaseType(i);

							if(base.getType() == TypeEntity::Type::Alias)
							{
								auto underlying = static_cast <TypeAliasEntity&> (base).getUnderlying(true);
								assert(underlying);

								initializeBaseContext(classEntity, *underlying);
							}

							else
							{
								initializeBaseContext(classEntity, base);
							}
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
