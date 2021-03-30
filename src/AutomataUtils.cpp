#include <string>
#include <stdio.h>
#include <dirent.h>

void deleteFiles(const std::string& folderpath)
{
	DIR* theFolder = opendir(folderpath.c_str());
	struct dirent* next_file;
	char filepath[512];

	while ((next_file = readdir(theFolder))!=nullptr) {
		sprintf(filepath, "%s/%s", folderpath.c_str(), next_file->d_name);
		remove(filepath);
	}
	closedir(theFolder);
}