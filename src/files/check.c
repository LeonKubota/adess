#include <stdio.h>
#include <stdbool.h>
#include <dirent.h> // Probably OK on Linux/MacOS, need to test for W*ndows
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h> // for _getcwd
#define PATH_SEPARATOR "\\" // Windows' weird things (idiot Bill Gates)
#define getcwd _getcwd // so that its the same later
#else
#include <unistd.h> // for gecwd
#define PATH_SEPARATOR "/" // for sane people
#endif

#include "utils.h"

bool checkFileExists(char *path, char *filename) {
	struct dirent *entry;
	DIR *dir = opendir(path);

	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry->d_name, filename)) {
			return true;
		}
	}

	return false;
}

char *getCurDirectory(char *userpath) {
	char cwd[4096]; // Should be enough (even overkill)
	
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		if (userpath == NULL) {
			return strcat(cwd, PATH_SEPARATOR);
		} else {
			return strcat(strcat(strcat(cwd, PATH_SEPARATOR), userpath), PATH_SEPARATOR);
		}
	} else {
		e_fatal("directory not found\n");
	}
	return NULL;
}
