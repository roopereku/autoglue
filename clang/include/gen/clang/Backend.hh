#ifndef GEN_CLANG_BACKEND_HH
#define GEN_CLANG_BACKEND_HH

#include <gen/Backend.hh>
#include <gen/FileList.hh>
#include <gen/TypeEntity.hh>
#include <gen/ScopeEntity.hh>
#include <gen/FunctionEntity.hh>

#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>

namespace gen::clang
{

class Backend : public gen::Backend
{
public:
	Backend(FileList& headers);

	bool loadCompilationDatabase(std::string_view directoryPath);
	bool generateHierarchy() override;

	Entity& getRoot() override;

private:
	/// Tries to ensure that the hierarchy specified by a cursor USR exists.
	std::shared_ptr <Entity> ensureHierarchyExists(CXCursor cursor);

	std::shared_ptr <TypeEntity> resolveType(CXCursor cursor);
	std::shared_ptr <TypeEntity> resolveType(CXType type);

	std::shared_ptr <ScopeEntity> global;
	CXCompilationDatabase compilationDatabase;
	CXIndex index;
};

}

#endif
