#include <stdio.h>
#include <stdbool.h>
#include <dirent.h> // Probably OK on Linux/MacOS, need to test for W*ndows
#include <string.h>

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
