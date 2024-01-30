#include <gen/clang/GlueGenerator.hh>
#include <gen/clang/TypeContext.hh>
#include <gen/clang/Backend.hh>

#include <gen/TypeReferenceEntity.hh>

namespace gen::clang
{

GlueGenerator::GlueGenerator(Backend& backend)
	: BindingGenerator(backend), file("glue.cc")
{
}

void GlueGenerator::generateClassBeginning(ClassEntity& entity)
{
}

void GlueGenerator::generateClassEnding(ClassEntity& entity)
{
}

void GlueGenerator::generateEnumBeginning(EnumEntity& entity)
{
}

void GlueGenerator::generateEnumEnding(EnumEntity& entity)
{
}

void GlueGenerator::generateEnumEntry(EnumEntryEntity& entity)
{
}

void GlueGenerator::generateFunction(FunctionEntity& entity)
{
	file << "extern \"C\"\n";
	file << "void " << entity.getHierarchy() << "(";

	if(entity.needsThisHandle())
	{
		file << "void* thisPtr";

		if(entity.getParameterCount() > 0)
		{
			file << ", ";
		}
	}

	entity.generateParameters(*this);
	file << ")\n{\n";

	switch(entity.getType())
	{
		case FunctionEntity::Type::Constructor:
		{
			file << "return new " << entity.getParent().getHierarchy("::");
			break;
		}

		case FunctionEntity::Type::Destructor:
		{
			file << "delete static_cast <" << entity.getParent().getHierarchy("::")
				<< "*> (thisPtr)";

			// Parameters aren't used for the destructor.
			return;
		}

		case FunctionEntity::Type::MemberFunction:
		{
			file << "static_cast <" << entity.getParent().getHierarchy("::")
				<< "*> (thisPtr)->" << entity.getName();
			break;
		}

		case FunctionEntity::Type::Function:
		{
			break;
		}
	}

	file << '(';
	onlyParameterNames = true;
	entity.generateParameters(*this);
	onlyParameterNames = false;
	file << ");\n";

	file << "}\n\n";
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		if(entity.isClass())
		{
			file << "static_cast <" << entity.getReferred().getHierarchy("::")
				<< "*> (" << entity.getName() << ')';
			return;
		}

		file << entity.getName();
	}

	else
	{
		if(entity.isClass())
		{
			file << "void* " << entity.getName();
			return;
		}

		if(entity.isEnum())
		{
			file << "int " << entity.getName();
			return;
		}
	}
}

void GlueGenerator::generateNamedScopeBeginning(ScopeEntity& entity)
{
}

void GlueGenerator::generateNamedScopeEnding(ScopeEntity& entity)
{
}

void GlueGenerator::generateArgumentSeparator()
{
	file << ", ";
}

}
