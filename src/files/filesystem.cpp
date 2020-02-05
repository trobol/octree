#include "filesystem.h"

#include <vector>
#include <iostream>
#include <dirent.h>

namespace filesystem
{
std::string fileSelect(std::string path, std::string ext)
{
	std::string result;

	dirent **files;
	int i;
	int n;

	/* Scan files in directory */
	n = scandir(path.c_str(), &files, NULL, alphasort);
	if (n == -1)
	{
		fprintf(stderr, "Cannot open %s (%s)\n", path.c_str(), strerror(errno));
		return "";
	}
	std::cout << "Pick " << ext << " file from: " << path << " (" << n << ")" << std::endl;
	/* Loop through file names */
	for (i = 0; i < n; i++)
	{
		struct dirent *ent;
		/* Get pointer to file entry */
		ent = files[i];

		printf("[%u]: ", i);
		/* Output file name */
		switch (ent->d_type)
		{
		case DT_REG:
			printf("%s\n", ent->d_name);
			break;

		case DT_DIR:
			printf("%s/\n", ent->d_name);
			break;

		case DT_LNK:
			printf("%s@\n", ent->d_name);
			break;

		default:
			printf("%s*\n", ent->d_name);
		}
	}

	int index = -1;
	while (index < 0 || index >= n)
	{
		std::cout << "Enter index: ";
		std::cin >> index;
	}
	result = path + files[index]->d_name;
	/* Release file names */
	for (i = 0; i < n; i++)
	{
		free(files[i]);
	}
	free(files);

	return result;
}

} // namespace filesystem