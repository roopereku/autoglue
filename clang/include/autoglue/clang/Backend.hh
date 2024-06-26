#ifndef AUTOGLUE_CLANG_BACKEND_HH
#define AUTOGLUE_CLANG_BACKEND_HH

#include <autoglue/Backend.hh>
#include <autoglue/ClassEntity.hh>
#include <autoglue/ScopeEntity.hh>
#include <autoglue/FunctionEntity.hh>

#include <clang/Tooling/JSONCompilationDatabase.h>

namespace ag::clang
{

class Backend : public ag::Backend
{
public:
	Backend(std::string_view compilationDatabasePath);

	bool generateHierarchy() override;

	std::string getInclusion(const std::string& path);

protected:
	void generateGlue() override;

private:
	void disableUntrivialNew(ClassEntity& entity);

	std::unique_ptr <::clang::tooling::JSONCompilationDatabase> database;
	std::vector <std::string> includePaths;
};

}

#endif
