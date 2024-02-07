#ifndef GEN_CLANG_BACKEND_HH
#define GEN_CLANG_BACKEND_HH

#include <gen/Backend.hh>
#include <gen/FileList.hh>
#include <gen/TypeEntity.hh>
#include <gen/ScopeEntity.hh>
#include <gen/FunctionEntity.hh>

#include <clang/Tooling/JSONCompilationDatabase.h>

namespace gen::clang
{

class Backend : public gen::Backend
{
public:
	Backend(FileList& headers);

	bool loadCompilationDatabase(std::string_view path);
	bool generateHierarchy() override;

private:
	std::unique_ptr <::clang::tooling::JSONCompilationDatabase> database;
};

}

#endif
