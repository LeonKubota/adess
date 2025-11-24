#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> // For the 'mkdir' function

// For reading files
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
#include "commands/command.h"
#include "commands/make.h"

int make(char *arg) {
	d_showInput("make", arg);
	
	// If there is an argument
	if (arg) {
		// project
		if (strcmp(arg, "project") == 0) {
			project();
		}
		// engine
		else if (strcmp(arg, "engine") == 0) {
			printf("engine\n");
		}
		// scene
		else if (strcmp(arg, "scene") == 0) {
			printf("scene\n");
		}
		// key
		else if (strcmp(arg, "key") == 0) {
			printf("key\n");
		}
		// Error
		else {
			e_fatal("incorrect component '%s'\n", arg);
			return 1;
		}
	} else {
		e_fatal("no component provided\n");
		return 1;
	}
	return 0;
}

int project() {
	// Check if name is provided
	if (strcmp(g_vals[1][0], "") == 0) {
		e_fatal("name is required to create project\n");
		return 1;
	}

	char *name = g_vals[1][0];

	char *userpath= NULL;
	// Check if directory/directories are provided. Set userpath to correct value or exit if multiple values are given. (null) value is allowed.
	for (int val = 0; val < MAX_VAL_COUNT; val++) {
		if (strcmp(g_vals[3][val], "") != 0) {
			if (userpath == NULL) {
				userpath = g_vals[3][val];
			} else {
				e_fatal("cannot create project with multiple directories\n");
				return 1;
			}
		}
	}

	char *path = getCurDirectory(userpath);
	path = strcat(path, name);

	makeDirectory(path);
	//makeFile(path);
	
	return 0;
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
		return NULL;
	}
}

int makeDirectory(char *path) {
	// Check if directory already exists
	if (access(path, F_OK) != -1) {
		e_fatal("could not create directory '%s', already exists\n", path);
		return 1;
	}

	// 0755 is for permissions, it is extremely strange
	if (mkdir(path, 0755) != 0) {
		e_fatal("could not create directory at: 's'\n", path);
		return 1;
	}

	n_out("directory created successfully at: '%s'\n", path);
	
	return 0;
}

// This is system specific
int makeFile(char *path) {
	// Check if the file already exists
	if (access(path, F_OK) != -1) {
		e_fatal("could not create file '%s', already exists\n", path);
		return 1;
	}

	FILE *file;
	file = fopen(path, "a");

	if (file == NULL) {
		e_fatal("could not create file at: '%s'\n", path);
		return 1;
	}
	
	fclose(file);

	return 0;
}
