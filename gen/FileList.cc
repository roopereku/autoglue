#include <gen/FileList.hh>

namespace gen
{

FileList::FileList(std::filesystem::path rootDirectory)
{
	// Recursively iterate the given directory.
	for(const auto& path : std::filesystem::recursive_directory_iterator(rootDirectory))
	{
		// Save regular files.
		if(path.is_regular_file())
		{
			files.emplace_back(path);
		}
	}
}


FileList::ContainerType::iterator FileList::begin()
{
	return files.begin();
}

FileList::ContainerType::iterator FileList::end()
{
	return files.end();
}

}
