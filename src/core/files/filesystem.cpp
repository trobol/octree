#include <octree/core/files/filesystem.h>

#include <vector>
#include <iostream>
#ifdef WIN32
#include <win_dirent.h>
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <string>

using std::string;

namespace filesystem
{

	/* Returns a list of files in a directory (except the ones that begin with a dot) */
	// src https://stackoverflow.com/a/1932861/8781351
	void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory)
	{
	#ifdef WINDOWS
		HANDLE dir;
		WIN32_FIND_DATA file_data;

		if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
			return; /* No files found */

		do {
			const string file_name = file_data.cFileName;
			const string full_file_name = directory + "/" + file_name;
			const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

			if (file_name[0] == '.')
				continue;

			if (is_directory)
				continue;

			out.push_back(full_file_name);
		} while (FindNextFile(dir, &file_data));

		FindClose(dir);
	#else
		DIR *dir;
		struct dirent *ent;
		struct stat st;

		dir = opendir(directory.c_str());
		while ((ent = readdir(dir)) != NULL) {
			const string file_name = ent->d_name;
			const string full_file_name = directory + "/" + file_name;

			if (file_name[0] == '.')
				continue;

			if (stat(full_file_name.c_str(), &st) == -1)
				continue;

			const bool is_directory = (st.st_mode & S_IFDIR) != 0;

			if (is_directory)
				continue;

			out.push_back(full_file_name);
		}
		closedir(dir);
	#endif
	} // GetFilesInDirectory


	std::string fileSelect(std::string path, std::string ext)
	{
		std::string result;
		
		std::vector<std::string> files;

		GetFilesInDirectory(files, path);
	

		std::cout << "Pick " << ext << " file from: " << path << " (" << files.size() << ")" << std::endl;
		
		for(int i = 0; i < files.size(); i++) {
			printf("[%i]: %s\n", i, files[i].c_str());
		}

		int index = -1;
		while (index < 0 || index >= files.size())
		{
			std::cout << "Enter index: " << std::flush;
			std::cin >> index;
		}
		result = files[index];

		return result;
	}

} // namespace filesystem