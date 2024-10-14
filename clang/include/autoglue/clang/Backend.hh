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
	Backend(std::unique_ptr <Deserializer>&& deserializer);

	std::string getInclusion(const std::string& path);

protected:
	bool onGenerateHierarchy() override;
	void onGenerateGlue() override;

private:
	void disableUntrivialNew(ClassEntity& entity);

	std::unique_ptr <::clang::tooling::JSONCompilationDatabase> database;
	std::vector <std::string> includePaths;
};

}

#endif
