#include <gen/clang/Backend.hh>
#include <gen/FunctionEntity.hh>
#include <gen/ParameterEntity.hh>
#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cctype>
#include <set>

namespace gen::clang
{

Backend::Backend(FileList& headers) : index(clang_createIndex(0, 1))
{
	std::ofstream inclusionFile("inclusions.cc");
	global = std::make_shared <ScopeEntity> ();

	// Write the header inclusions.
	for(auto& path : headers)
	{
		inclusionFile << "#include " << path << '\n';
	}
}

Entity& Backend::getRoot()
{
	return *global;
}

bool Backend::loadCompilationDatabase(std::string_view directoryPath)
{
	CXCompilationDatabase_Error error;
	compilationDatabase = clang_CompilationDatabase_fromDirectory(std::string(directoryPath).c_str(), &error);

	if(error == CXCompilationDatabase_CanNotLoadDatabase)
	{
		std::cerr << "Unable to load a compilation database from " << directoryPath << '\n';
		return false;
	}

	return true;
}

bool Backend::generateHierarchy()
{
	auto compileCommands = clang_CompilationDatabase_getAllCompileCommands(compilationDatabase);
	std::vector <const char*> clangArgs;

	// Iterate all compilation comands.
	for(unsigned i = 0; i < clang_CompileCommands_getSize(compileCommands); i++)
	{
		bool ignoreNext = false;

		auto command = clang_CompileCommands_getCommand(compileCommands, i);
		for(unsigned a = 0; a < clang_CompileCommand_getNumArgs(command); a++)
		{
			auto arg = clang_CompileCommand_getArg(command, a);
			std::string_view str(clang_getCString(arg));

			if(ignoreNext)
			{
				ignoreNext = false;
			}

			else if(str == "-o" || str == "-c")
			{
				ignoreNext = true;
			}

			else
			{
				//std::cout << "Clang arg " << str << '\n';
				clangArgs.emplace_back(str.data());
			}
				
			//clang_disposeString(arg);
		}
	}

	// Tell clang to compile the headers.
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index, "inclusions.cc",
		clangArgs.data(), clangArgs.size(),
		nullptr, 0,
		CXTranslationUnit_None
	);

	if(!unit)
	{
		std::cerr << "Failed to parse\n";
		return false;
	}

	// Start traversing the AST.
	clang_visitChildren(
		clang_getTranslationUnitCursor(unit),

		[](CXCursor cursor, CXCursor parent, CXClientData data)
		{
			auto backend = static_cast <Backend*> (data);

			switch(cursor.kind)
			{
				// For classes and functions make sure that the entity hierarchy exists.
				case CXCursorKind::CXCursor_ClassDecl:
				case CXCursorKind::CXCursor_StructDecl:
				case CXCursorKind::CXCursor_CXXMethod:
				{
					if(!backend->ensureHierarchyExists(cursor))
					{
						return CXChildVisit_Continue;
					}

					break;
				}

				case CXCursorKind::CXCursor_ParmDecl:
				{
					if(!backend->ensureHierarchyExists(cursor))
					{
						return CXChildVisit_Continue;
					}

					break;
				}

				default:
				{
				}
			}

			return CXChildVisit_Recurse;
		},

		this
	);

	// Generation is succesful if the global scope still exists.
	return static_cast <bool> (global);
}

std::shared_ptr <Entity> Backend::ensureHierarchyExists(CXCursor cursor)
{
	// Translation unit indicates the global scope.
	if(cursor.kind == CXCursorKind::CXCursor_TranslationUnit)
	{
		return global;
	}

	// Make sure that the containing entity exists.
	auto parent = clang_getCursorSemanticParent(cursor);
	auto parentEntity = ensureHierarchyExists(parent);

	if(!parentEntity)
	{
		return nullptr;
	}

	// Check if the current entity already exists within the parent.
	auto spelling = clang_getCursorSpelling(cursor);
	auto entity = parentEntity->resolve(clang_getCString(spelling));

	// If the current entity doesn't exist, create it.
	if(!entity)
	{
		switch(cursor.kind)
		{
			// Structs and classes become class entities.
			case CXCursorKind::CXCursor_ClassDecl:
			case CXCursorKind::CXCursor_StructDecl:
			{
				parentEntity->addChild(std::make_shared <ClassEntity> (clang_getCString(spelling)));
				break;
			}

			// Namespaces become named scope entities.
			case CXCursorKind::CXCursor_Namespace:
			{
				parentEntity->addChild(std::make_shared <ScopeEntity> (clang_getCString(spelling)));
				break;
			}

			// Anything that's a function becomes a function entity.
			case CXCursorKind::CXCursor_CXXMethod:
			case CXCursorKind::CXCursor_Constructor:
			case CXCursorKind::CXCursor_Destructor:
			case CXCursorKind::CXCursor_FunctionDecl:
			{
				parentEntity->addChild(std::make_shared <FunctionEntity> (clang_getCString(spelling)));
				break;
			}

			// Create parameter entities and associate them with the appropriate type.
			case CXCursorKind::CXCursor_ParmDecl:
			{
				auto paramType = resolveType(cursor);

				parentEntity->addChild(std::make_shared <ParameterEntity> (clang_getCString(spelling), paramType));
				break;
			}

			default:
			{
				auto cursorKind = clang_getCursorKindSpelling(cursor.kind);
				//std::cerr << "Unimplemented ensuring for cursor kind " << clang_getCString(cursorKind) << '\n';
				clang_disposeString(cursorKind);
			}
		}

		// After adding, try to resolve the entity again.
		entity = parentEntity->resolve(clang_getCString(spelling));
	}

	clang_disposeString(spelling);
	return entity;
}

std::shared_ptr <ClassEntity> Backend::resolveType(CXCursor cursor)
{
	auto cursorType = clang_getCanonicalType(clang_getCursorType(cursor));

	// Remove references.
	cursorType = clang_getNonReferenceType(cursorType);

	// Get the type declaration and do special handling if it's not found.
	auto declaration = clang_getTypeDeclaration(cursorType);
	if(declaration.kind == CXCursorKind::CXCursor_NoDeclFound)
	{
		// Remove qualifiers such as const. Explicitly do this only for types without
		// a declaration as getting the type declaration should do the same.
		cursorType = clang_getUnqualifiedType(cursorType);

		// Remove pointers.
		while(cursorType.kind == CXTypeKind::CXType_Pointer)
		{
			cursorType = clang_getPointeeType(cursorType);
		}

		// Save the type spelling into an editable string.
		auto spelling = clang_getTypeSpelling(cursorType);
		std::string typeName(clang_getCString(spelling));
		clang_disposeString(spelling);

		//if(clang_Type_getNumTemplateArguments(cursorType) != -1)
		//{
		//	std::cout << "Template " << typeName << '\n';
		//}

		// Replace spaces and namespace resolutions with underscores.
		std::replace(typeName.begin(), typeName.end(), ' ', '_');
		std::replace(typeName.begin(), typeName.end(), ':', '_');

		// Check if this type was already added.
		auto result = global->resolve(typeName);
		if(!result)
		{
			// If the type can't be found in the global scope, add it.
			global->addChild(std::make_shared <ClassEntity> (typeName));
			result = global->resolve(typeName);
		}

		return std::static_pointer_cast <ClassEntity> (result);
	}

	auto declKind = clang_getCursorKindSpelling(declaration.kind);
	auto declUsr = clang_getCursorUSR(declaration);

	//std::cout << "Declaration " << clang_getCString(declUsr) << " is of kind " << clang_getCString(declKind) << '\n';
	clang_disposeString(declKind);
	clang_disposeString(declUsr);

	// Get the type as an entity.
	return std::static_pointer_cast <ClassEntity> (ensureHierarchyExists(declaration));
}

}
