#include <gen/clang/GlueGenerator.hh>
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

	if(getClassDepth() > 0)
	{
		file << "void* thisPtr";

		if(entity.getParameterCount() > 0)
		{
			file << ", ";
		}
	}

	entity.generateParameters(*this);

	file << ")\n{\n";
	file << "}\n\n";
}

void GlueGenerator::generateTypeReference(TypeReferenceEntity& entity)
{
	if(onlyParameterNames)
	{
		file << entity.getName();
	}

	else
	{
		file << entity.getType().getName() << ' ' << entity.getName();
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
