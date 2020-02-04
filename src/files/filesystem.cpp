#include "filesystem.h"

#include <vector>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

namespace filesystem
{
	std::string fileSelect(std::string path, std::string ext)
	{

		std::vector<std::string> files;
		for (const auto& entry : fs::directory_iterator(path)) {
			std::cout << entry.path().string() << '\n';
			if (!entry.is_directory() && (ext.empty() || ext == entry.path().extension())) {
				
				files.push_back(entry.path().string());
			}
		}

		std::cout << "Pick " << ext << " file from: " << path << " (" << files.size() << ")" << std::endl;
		for (int i = 0; i < files.size(); i++) {
			std::cout << "[" << i << "]  " << files[i] << '\n';
		}
		std::cout << std::flush;
		int index = -1;
		while (index < 0 || index >= files.size()) {
			std::cout << "Enter index: ";
			std::cin >> index;
		}

		return files[index];
	}

} // namespace filesystem