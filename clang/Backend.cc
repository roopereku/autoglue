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

	auto parent = clang_getCursorSemanticParent(cursor);
	auto parentEntity = ensureHierarchyExists(parent);

	if(!parentEntity)
	{
		return nullptr;
	}

	auto spelling = clang_getCursorSpelling(cursor);
	auto entity = parentEntity->resolve(clang_getCString(spelling));

	if(!entity)
	{
		switch(cursor.kind)
		{
			case CXCursorKind::CXCursor_ClassDecl:
			case CXCursorKind::CXCursor_StructDecl:
			{
				parentEntity->addChild(std::make_shared <ClassEntity> (clang_getCString(spelling)));
				break;
			}

			case CXCursorKind::CXCursor_Namespace:
			{
				parentEntity->addChild(std::make_shared <ScopeEntity> (clang_getCString(spelling)));
				break;
			}

			case CXCursorKind::CXCursor_CXXMethod:
			case CXCursorKind::CXCursor_Constructor:
			case CXCursorKind::CXCursor_Destructor:
			case CXCursorKind::CXCursor_FunctionDecl:
			{
				parentEntity->addChild(std::make_shared <FunctionEntity> (clang_getCString(spelling)));
				break;
			}

			case CXCursorKind::CXCursor_ParmDecl:
			{
				parentEntity->addChild(std::make_shared <ParameterEntity> (clang_getCString(spelling)));
				break;
			}

			default:
			{
				auto cursorKind = clang_getCursorKindSpelling(cursor.kind);
				std::cerr << "Unimplemented ensuring for cursor kind " << clang_getCString(cursorKind) << '\n';
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

	//// Remove pointers.
	//if(cursorType.kind == CXTypeKind::CXType_Pointer)
	//{
	//	cursorType = clang_getPointeeType(cursorType);
	//}

	// Remove references.
	cursorType = clang_getNonReferenceType(cursorType);

	// Remove modifiers.
	cursorType = clang_getUnqualifiedType(cursorType);

	auto spelling = clang_getTypeSpelling(cursorType);
	//std::cout << "Get declaration for " << clang_getCString(spelling) << '\n';
	clang_disposeString(spelling);

	auto declaration = clang_getTypeDeclaration(cursorType);

	auto declKind = clang_getCursorKindSpelling(declaration.kind);
	auto declUsr = clang_getCursorUSR(declaration);
	std::cout << "Declaration " << clang_getCString(declUsr) << " is of kind " << clang_getCString(declKind) << '\n';
	clang_disposeString(declKind);
	clang_disposeString(declUsr);

	// Get the type as an entity.
	return std::static_pointer_cast <ClassEntity> (ensureHierarchyExists(declaration));
}

}
