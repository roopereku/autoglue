#include <gen/clang/Backend.hh>
#include <gen/FunctionEntity.hh>
#include <gen/ClassEntity.hh>
#include <gen/EnumEntity.hh>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>

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

bool Backend::generateHierarchy()
{
	// Tell clang to compile the headers.
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index, "inclusions.cc",
		nullptr, 0,
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
			switch(cursor.kind)
			{
				case CXCursorKind::CXCursor_ClassDecl:
				case CXCursorKind::CXCursor_StructDecl:
				case CXCursorKind::CXCursor_CXXMethod:
				{
					auto backend = static_cast <Backend*> (data);
					auto usr = clang_getCursorUSR(cursor);

					// If ensuring the hierarchy fails, stop traversing.
					if(!backend->ensureHierarchyExists(usr))
					{
						backend->global = nullptr;
						return CXChildVisit_Break;
					}

					clang_disposeString(usr);
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

void Backend::exclude(std::string_view name)
{
	nameExclusions.emplace_back(name);
}

bool Backend::ensureHierarchyExists(CXString usr)
{
	std::string_view str(clang_getCString(usr));

	// Ignore c:@.
	str = str.substr(str.find('@') + 1);

	if(!ensureEntityExists(str, global))
	{
		std::cerr << "Ensuring " << str << " failed\n";
		return false;
	}

	return true;
}

bool Backend::ensureEntityExists(std::string_view usr, std::shared_ptr <Entity> from)
{
	size_t nameBegin = usr.find('@') + 1;
	size_t nameEnd = usr.find('@', nameBegin);

	bool recurse = true;

	// Get the type and name of the current entity.
	std::string_view entityType(usr.substr(0, nameBegin - 1));
	std::string_view entityName(usr.substr(nameBegin, nameEnd - nameBegin));

	// For functions remove the parameter information (Anything after the first "#").
	// NOTE: ensureEntityExists shouldn't be called after this.
	// TODO: Do this for classes as well.
	if(entityType == "F")
	{
		size_t nextHash = entityName.find('#');

		if(nextHash != std::string_view::npos)
		{
			entityName = entityName.substr(0, nextHash);
			recurse = false;
		}
	}

	// Strip anything after a dot and the dot in order to not confuse Entity::resolve().
	size_t nextDot = entityName.find('.');
	if(nextDot != std::string_view::npos)
	{
		entityName = entityName.substr(0, nextDot);
	}

		// Fake an existing entity if it has a name that should be ignored.
	if(std::find(nameExclusions.begin(), nameExclusions.end(), entityName) != nameExclusions.end())
	{
		return true;
	}

	// Try to resolve an existing entity.
	auto result = from->resolve(entityName);

	// If given entity doesn't exist, try to create it.
	if(!result)
	{
		if(entityType.size() > 1)
		{
			std::cerr << "Unimplemented: Multicharacter USR type " << entityType << " in " << usr << '\n';
			return true;
		}

		switch(usr[0])
		{
			// "S" indicates a class or a struct.
			case 'S':
			{
				from->addChild(std::make_shared <ClassEntity> (entityName));
				break;
			}

			// "E" indicates an enum.
			case 'E':
			{
				from->addChild(std::make_shared <EnumEntity> (entityName));
				break;
			}

			// "F" indicates a function.
			case 'F':
			{
				from->addChild(std::make_shared <FunctionEntity> (entityName));
				break;
			}

			// "N" indicates a namespace.
			case 'N':
			{
				from->addChild(std::make_shared <ScopeEntity> (entityName));
				break;
			}

			default:
			{
				std::cerr << "Unimplemented: USR type " << usr[0] << " in " << usr << '\n';
				return true;
			}
		}

		// Try to resolve the newly created entity.
		result = from->resolve(entityName);

		if(!result)
		{
			std::cerr << "Resolve after adding " << entityType << " " << entityName << " failed\n";
			return false;
		}
	}

	// If there are no more "@" symbols, this is the end.
	if(nameEnd == std::string_view::npos || !recurse)
	{
		return true;
	}

	// Go to the next portion of the usr and make sure that it exists.
	usr = usr.substr(nameEnd + 1);
	return ensureEntityExists(usr, result);
}

}
