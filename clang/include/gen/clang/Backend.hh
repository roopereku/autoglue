#ifndef GEN_CLANG_BACKEND_HH
#define GEN_CLANG_BACKEND_HH

#include <gen/Backend.hh>
#include <gen/FileList.hh>
#include <gen/ScopeEntity.hh>
#include <clang-c/Index.h>

namespace gen::clang
{

class Backend : public gen::Backend
{
public:
	Backend(FileList& headers);

	Entity& getRoot() override;

private:
	void ensureHierarchyExists(CXString usr);

	ScopeEntity global;
	CXIndex index;
};

}

#endif
