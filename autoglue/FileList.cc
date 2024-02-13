#include <autoglue/FileList.hh>

namespace ag
{

FileList::FileList(std::filesystem::path rootDirectory, std::string_view extension)
{
	// Recursively iterate the given directory.
	for(const auto& path : std::filesystem::recursive_directory_iterator(rootDirectory))
	{
		// Save regular files.
		if(path.is_regular_file())
		{
			// If the extension is filtered, only save paths that have the extension.
			if(!extension.empty() && path.path().extension() != extension)
			{
				continue;
			}

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
