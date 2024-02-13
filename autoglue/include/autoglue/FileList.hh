#ifndef AUTOGLUE_FILE_LIST_HH
#define AUTOGLUE_FILE_LIST_HH

#include <string_view>
#include <filesystem>
#include <vector>

namespace ag
{

class FileList
{
public:
	using ContainerType = std::vector <std::filesystem::path>;

	FileList(std::filesystem::path rootDirectory, std::string_view extension = "");

	ContainerType::iterator begin();
	ContainerType::iterator end();

private:
	ContainerType files;
};

}

#endif
