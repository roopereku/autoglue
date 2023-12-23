#ifndef GEN_FILE_LIST_HH
#define GEN_FILE_LIST_HH

#include <filesystem>
#include <vector>

namespace gen
{

class FileList
{
public:
	using ContainerType = std::vector <std::filesystem::path>;

	FileList(std::filesystem::path rootDirectory);

	ContainerType::iterator begin();
	ContainerType::iterator end();

private:
	ContainerType files;
};

}

#endif
