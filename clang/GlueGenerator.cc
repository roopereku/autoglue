#include <autoglue/clang/GlueGenerator.hh>
#include <autoglue/clang/Backend.hh>
#include <autoglue/clang/EntityContext.hh>
#include <autoglue/clang/TypeContext.hh>
#include <autoglue/clang/TyperefContext.hh>
#include <autoglue/clang/FunctionContext.hh>
#include <autoglue/clang/OverloadContext.hh>

#include <autoglue/TypeReferenceEntity.hh>
#include <autoglue/FunctionGroupEntity.hh>
#include <autoglue/FunctionEntity.hh>

#include <iostream>
#include <set>

namespace ag::clang
{

static std::shared_ptr <EntityContext> getClangContext(const Entity& entity)
{
	if(entity.getContext())
	{
		return std::static_pointer_cast <EntityContext> (entity.getContext());
	}

	return nullptr;
}

static bool stringNeedsDuplication(FunctionEntity& entity)
{
	// If this function is a constructor for std::string_view, the passed in strings
	// should be duplicated in order for the string_view to be able to reference them.
	// This has to be done because things like C#, Java and Python will destroy
	// a passed in c-string as soon as they don't need it.
	//
	// This is a bit hacky because std::string_view is supposed to be a
	// non-owning type, but what can you really do?
	//
	// TODO: Similiar things may need to be done for other referencing types?
	assert(entity.getType() == FunctionEntity::Type::Constructor ||
			entity.getType() == FunctionEntity::Type::Destructor);

	auto ctx = getClangContext(entity.getParent());
	return ctx && ctx->getTypeContext()->getRealName() == "std::basic_string_view<char,std::char_traits<char>>";
}

static std::string getSelfType(FunctionEntity& entity)
{
	// If this function was originally a C++ function, the backend should've initialized
	// a typeref context for the function group. This context would contain the full
	// type (including templates) of the "this type" for the function.
	//
	// If the context isn't present, just use the location provided by the hierarchy.
	auto ctx = getClangContext(entity.getGroup());
	return ctx ? std::string(ctx->getFunctionContext()->getSelfType()) : entity.getParent().getHierarchy("::");

}

void generateTypePOD(std::ofstream& file, TypeReferenceEntity& entity)
{
	switch(entity.getPrimitiveType().getType())
	{
		case PrimitiveEntity::Type::String: file << "const char *"; break;
		case PrimitiveEntity::Type::ObjectHandle: file << "void*"; break;
		case PrimitiveEntity::Type::Character: file << "char"; break;
		case PrimitiveEntity::Type::Double: file << "double"; break;
		case PrimitiveEntity::Type::Boolean: file << "bool"; break;
		case PrimitiveEntity::Type::Float: file << "float"; break;
		case PrimitiveEntity::Type::Integer: file << "int"; break;
		case PrimitiveEntity::Type::Void: file << "void"; break;
	}
}

class IncludeCollector : public BindingGenerator
{
public:
	IncludeCollector(Backend& backend)
		: BindingGenerator(backend)
	{
	}

	void generateNamedScope(ScopeEntity& entity) override
	{
		entity.generateNested(*this);
	}

	void generateClass(ClassEntity& entity) override
	{
		auto ctx = getClangContext(entity);
		if(ctx)
		{
			addInclude(ctx->getTypeContext()->getInclude());
		}

		entity.generateNested(*this);
	}

	void generateFunction(FunctionEntity& entity) override
	{
		entity.generateReturnType(*this, true);
		entity.generateParameters(*this, true, true);
	}

	void generateTypeReference(TypeReferenceEntity& entity) override
	{
		if(!entity.isPrimitive())
		{
			auto ctx = getClangContext(entity.getReferred());
			if(ctx)
			{
				includes.emplace("#include <" + ctx->getTypeContext()->getInclude() + ">");
			}
		}
	}

	void addInclude(const std::string& include)
	{
		if(!include.empty())
		{
			includes.emplace("#include <" + include + ">");
		}
	}

	std::set <std::string> includes;
};

class ClassGenerator : public ag::BindingGenerator
{
public:
	ClassGenerator(Backend& backend, std::ofstream& file)
		: BindingGenerator(backend), file(file)
	{
	}

	void generateNamedScope(ScopeEntity& entity) override
	{
		file << "namespace AG_" << entity.getName() << "\n{\n";
		entity.generateNested(*this);
		file << "}\n";
	}

	void generateClass(ClassEntity& entity) override
	{
		if(!inOverride)
		{
			file << "struct " << "AG_" << entity.getName();

			auto ctx = getClangContext(entity);
			if(ctx)
			{
				file << " : public " << ctx->getTypeContext()->getRealName();
			}

			file << "\n{\n";
		}

		auto constructors = entity.resolve("Constructor");
		if(constructors)
		{
			inOverride = true;
			constructors->generate(*this);
			constructors->resetGenerated();
			inOverride = false;
		}

		if(!inOverride)
		{
			inOverride = true;
			entity.generateConcreteType(*this);
			inOverride = false;
		}

		inBridge = true;
		entity.generateNested(*this);
		inBridge = false;

		if(!inOverride)
		{
			file << "};\n\n";
		}
	}

	void generateFunction(FunctionEntity& entity) override
	{
		if(entity.isInterface())
		{
			return;
		}

		if(inOverride)
		{
			if(entity.getType() == FunctionEntity::Type::Constructor)
			{
				file << "AG_" + entity.getParent().getName() << '(';
				entity.generateParameters(*this, false, false);
				file << ')';

				auto baseCtx = getClangContext(entity.getParent());
				if(baseCtx)
				{
					file << " : " << baseCtx->getTypeContext()->getRealName() << '(';

					onlyParameterNames = true;
					entity.generateParameters(*this, false, false);
					onlyParameterNames = false;

					file << ')';
				}

				file << "\n{\n";
				file << "}\n";
			}

			else if(entity.getType() == FunctionEntity::Type::MemberFunction)
			{
				auto ctx = getClangContext(entity);

				entity.generateReturnType(*this, false);
				file << entity.getName() << '(';
				entity.generateParameters(*this, false, false);
				file << ") " << ctx->getOverloadContext()->getEastQualifiers() << "override\n{\n";

				file << "}\n";
			}
		}

		else if(inBridge)
		{
			file << "static ";
			entity.generateReturnType(*this, true);
			file << entity.getBridgeName(true) << '(';
			entity.generateParameters(*this, true, true);
			file << ")\n{\n    ";

			bool closeParenthesis = entity.generateReturnStatement(*this, false);

			entity.generateBridgeCall(*this);

			if(closeParenthesis)
			{
				file << ')';
			}

			file << ";\n}\n";
		}
	}

	void generateBridgeCall(FunctionEntity& entity) override
	{
		switch(entity.getType())
		{
			case FunctionEntity::Type::MemberFunction:
			{
				file << "static_cast <AG_" << entity.getParent().getHierarchy("::AG_") << "*> (" <<
						getObjectHandleName() << ")->" << getSelfType(entity) <<
						"::" << entity.getName() << '(';

				onlyParameterNames = true;
				entity.generateParameters(*this, false, false);
				onlyParameterNames = false;

				file << ')';
				break;
			}

			case FunctionEntity::Type::Constructor:
			{
				file <<  "AG_" + entity.getParent().getHierarchy("::AG_") << '(';

				duplicateString = stringNeedsDuplication(entity);

				onlyParameterNames = true;
				castPrimitives = true;
				entity.generateParameters(*this, false, false);
				onlyParameterNames = false;
				castPrimitives = false;

				duplicateString = false;

				file << ')';
				break;
			}

			case FunctionEntity::Type::Destructor:
			{
				// If strdup was called for a std::string_view parameter, delete the duplicated string.
				if(stringNeedsDuplication(entity))
				{
					file << "free(const_cast <char*> (static_cast <AG_" << entity.getParent().getHierarchy("::AG_") <<
							"*> (" << getObjectHandleName() << ")->data()));\n";
				}

				file << "delete static_cast <AG_" << entity.getParent().getHierarchy("::AG_") <<
						"*> (" << getObjectHandleName() << ')';

				break;
			}

			case FunctionEntity::Type::Function:
			{
				// Nothing is generated in-class for a non-class method.
				break;
			}
		}
	}

	bool generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target) override
	{
		if(inBridge)
		{
			switch(entity.getType())
			{
				case TypeEntity::Type::Class:
				{
					// Allocate nothing when returning references.
					if(entity.isReference())
					{
						auto ctx = getClangContext(entity);
						assert(ctx);

						// If the returned reference object is not a pointer, take its address.
						if(!ctx->getTyperefContext()->isPointer())
						{
							file << '&';
						}

						return false;
					}

					// TODO: What if the "operator new" is protected and new is invoked
					// and this class has no access to it (Class of different type).
					file << "return new ";

					if(target.getType() != FunctionEntity::Type::Constructor)
					{
						auto ctx = getClangContext(entity);
						assert(ctx);

						// If a non-constructor function is returning a non-reference object,
						// allocate and return a new copy of it.
						file << ctx->getTyperefContext()->getWrittenType() << '(';
						return true;
					}

					return false;
				}

				case TypeEntity::Type::Enum:
				{
					// TODO: Use whatever integral format the enum has specified.
					file << "return static_cast <int> (";
					return true;
				}

				case TypeEntity::Type::Primitive:
				{
					file << "return ";
					return false;
				}

				case TypeEntity::Type::Alias:
				{
					TypeReferenceEntity underlying("", entity.getAliasType().getUnderlying(true), entity.isReference());
					underlying.initializeContext(entity.getContext());
					return generateReturnStatement(underlying, target);
				}
			}
		}

		return false;
	}

	void generateTypeReference(TypeReferenceEntity& entity) override
	{
		// Type names and reference names.
		if(!onlyParameterNames)
		{
			if(inOverride)
			{
				auto ctx = getClangContext(entity);
				assert(ctx);

				file << ctx->getTyperefContext()->getOriginalType();
			}

			else if(inBridge)
			{
				assert(entity.isPrimitive());
				generateTypePOD(file, entity);
			}

			file << ' ' << entity.getName();
		}

		// Parameters passed in override functions.
		else if(inOverride)
		{
			auto ctx = getClangContext(entity);
			assert(ctx);

			if(ctx->getTyperefContext()->isRValueReference())
			{
				file << "std::move(" << entity.getName() << ')';
			}	

			else
			{
				file << entity.getName();
			}
		}

		// Parameters passed in bridge functions.
		else if(inBridge)
		{
			switch(entity.getType())
			{
				case TypeEntity::Type::Class:
				{
					auto ctx = getClangContext(entity);
					assert(ctx);

					bool closeParenthesis = false;
					if(ctx->getTyperefContext()->isRValueReference())
					{
						file << "std::move(";
						closeParenthesis = true;
					}

					// If the class type parameter isn't a reference or a pointer, dereference it.
					if(!entity.isReference() || !ctx->getTyperefContext()->isPointer())
					{
						file << '*';
					}

					file << "static_cast <" << ctx->getTyperefContext()->getWrittenType() <<
							"*> (" << entity.getName() << ')';

					if(closeParenthesis)
					{
						file << ')';
					}

					break;
				}

				case TypeEntity::Type::Enum:
				{
					auto ctx = getClangContext(entity);
					assert(ctx);

					file << "static_cast <" << ctx->getTyperefContext()->getWrittenType() <<
							"> (" << entity.getName() << ')';

					break;
				}

				case TypeEntity::Type::Primitive:
				{
					bool closeParenthesis = false;

					// If a string should be duplicated, pass it into stdup.
					if(entity.getPrimitiveType().getType() == PrimitiveEntity::Type::String && duplicateString)
					{
						file << "strdup(";
						closeParenthesis = true;
					}

					if(castPrimitives)
					{
						auto ctx = getClangContext(entity);
						assert(ctx);

						file << "static_cast <" << ctx->getTyperefContext()->getOriginalType() <<
								"> (" << entity.getName() << ')';
					}

					else
					{
						file << entity.getName();
					}

					if(closeParenthesis)
					{
						file << ')';
					}

					break;
				}

				case TypeEntity::Type::Alias:
				{
					TypeReferenceEntity underlying(entity.getName(), entity.getAliasType().getUnderlying(true), entity.isReference());
					underlying.initializeContext(entity.getContext());
					generateTypeReference(underlying);

					break;
				}
			}
		}
	}

	void generateArgumentSeparator() override
	{
		file << ", ";
	}

	std::string_view getObjectHandleName() override
	{
		return "objectHandle";
	}

private:
	std::ofstream& file;

	bool inBridge = false;
	bool inOverride = false;
	bool onlyParameterNames = false;
	bool castPrimitives = false;
	bool duplicateString = false;
};

GlueGenerator::GlueGenerator(Backend& backend)
	: BindingGenerator(backend), file("glue.cpp")
{
	IncludeCollector collector(backend);
	collector.generateBindings();

	for(auto& include : collector.includes)
	{
		file << include << '\n';
	}

	ClassGenerator classGen(backend, file);
	classGen.generateBindings();
}

void GlueGenerator::generateFunction(FunctionEntity& entity)
{
	if(entity.isInterface())
	{
		return;
	}

	file << "extern \"C\"\n";
	auto returnType = entity.getReturnType();
	auto returnCtx = getClangContext(returnType);

	entity.generateReturnType(*this, true);
	file << entity.getBridgeName() << "(";

	entity.generateParameters(*this, true, true);
	file << ")\n{\n";

	entity.generateReturnStatement(*this, true);
	entity.generateBridgeCall(*this);

	file << ";\n}\n\n";
}

void GlueGenerator::generateNamedScope(ScopeEntity& entity)
{
	entity.generateNested(*this);
}

void GlueGenerator::generateClass(ClassEntity& entity)
{
	file << "// ---------- Class " << entity.getHierarchy("::") << " : " << " ----------\n\n";
	entity.generateNested(*this);
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		file << entity.getName();
	}

	else
	{
		generateTypePOD(file, entity);
		file << ' ' << entity.getName();
	}
}

void GlueGenerator::generateArgumentSeparator()
{
	file << ", ";
}

std::string_view GlueGenerator::getObjectHandleName()
{
	return "objectHandle";
}

bool GlueGenerator::generateReturnStatement(TypeReferenceEntity& entity, FunctionEntity& target)
{
	file << "return ";
	return false;
}

void GlueGenerator::generateBridgeCall(FunctionEntity& target)
{
	switch(target.getType())
	{
		case FunctionEntity::Type::MemberFunction:
		case FunctionEntity::Type::Constructor:
		case FunctionEntity::Type::Destructor:
		{
			file << "AG_" << target.getParent().getHierarchy("::AG_") << "::" <<
					target.getBridgeName(true) << '(';

			onlyParameterNames = true;
			target.generateParameters(*this, true, true);
			onlyParameterNames = false;

			file << ')';
			break;
		}

		case FunctionEntity::Type::Function:
		{
			// TODO: Implement non-class methods.
			break;
		}
	}
}

}
