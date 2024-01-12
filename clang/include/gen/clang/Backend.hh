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

	bool generateHierarchy() override;

	void exclude(std::string_view name);

	Entity& getRoot() override;

private:
	bool ensureHierarchyExists(CXString usr);
	bool ensureEntityExists(std::string_view usr, std::shared_ptr <Entity> from);

	std::shared_ptr <ScopeEntity> global;
	std::vector <std::string> nameExclusions;

	CXIndex index;
};

}

#endif
