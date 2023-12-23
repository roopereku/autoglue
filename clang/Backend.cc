#include <gen/clang/Backend.hh>

#include <iostream>
#include <fstream>

namespace gen::clang
{

Backend::Backend(FileList& headers) : index(clang_createIndex(0, 1))
{
	std::ofstream inclusionFile("inclusions.cc");

	for(auto& path : headers)
	{
		inclusionFile << "#include " << path << '\n';
	}

	inclusionFile.close();

	CXTranslationUnit unit = clang_parseTranslationUnit(
		index, "inclusions.cc",
		nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None
	);

	if(!unit)
	{
		std::cerr << "Failed to parse\n";
		return;
	}

	std::cout << "Traverse\n";

	clang_visitChildren(
		clang_getTranslationUnitCursor(unit),

		[](CXCursor cursor, CXCursor parent, CXClientData data)
		{
			auto backend = static_cast <Backend*> (data);
			auto usr = clang_getCursorUSR(cursor);
			auto spelling = clang_getCursorSpelling(cursor);

			if(cursor.kind == CXCursorKind::CXCursor_ClassDecl)
			{
				std::cout << "Class " << clang_getCString(usr) << '\n';
			}

			clang_disposeString(usr);
			clang_disposeString(spelling);

			return CXChildVisit_Recurse;
		},

		this
	);
}

Entity& Backend::getRoot()
{
	return global;
}

void Backend::ensureHierarchyExists(CXString usr)
{
}

}
