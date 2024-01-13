#ifndef GEN_CLANG_BACKEND_HH
#define GEN_CLANG_BACKEND_HH

#include <gen/Backend.hh>
#include <gen/FileList.hh>
#include <gen/ScopeEntity.hh>
#include <gen/FunctionEntity.hh>
#include <clang-c/Index.h>

namespace gen::clang
{

class Backend : public gen::Backend
{
public:
	Backend(FileList& headers);

	bool generateHierarchy() override;

	Entity& getRoot() override;

private:
	/// Tries to ensure that the hierarchy specified by a cursor USR exists.
	std::shared_ptr <Entity> ensureHierarchyExists(CXCursor cursor);

	std::shared_ptr <Entity> ensureEntityExists(std::string_view usr, std::shared_ptr <Entity> from);

	std::shared_ptr <ScopeEntity> global;
	CXIndex index;
};

}

#endif
