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
#include "files/check.h"

bool checkFileExistsIn(char *path, char *filename) {
	struct dirent *entry;
	DIR *dir = opendir(path);

	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry -> d_name, filename)) {
			return true;
		}
	}

	return false;
}

bool checkFileExists(char *path) {
	FILE *file = fopen(path, "r");
	
	if (file != NULL) {
		return true;
	}

	return false;
}

char *findProjectFile(char *path) {
	// If there is no file ending in '.adess'
	if (!checkFileExistsIn(path, ".adess")) {
		e_fatal("project file not found in '%s'\n", path);
		return NULL;
	}

	struct dirent *entry;
	DIR *dir = opendir(path);
	char *projectFilePath = NULL;
	
	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry -> d_name, ".adess")) {
			if (projectFilePath == NULL) {
				projectFilePath = entry -> d_name;  
			} else {
				e_fatal("multiple projects file found in '%s'\n", path); 
				return NULL;
			}
		}
	}

	return strcat(path, projectFilePath);
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
