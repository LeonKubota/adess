#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> // For the 'mkdir' function
#include <stdarg.h>

// For reading files
#include <unistd.h> // for gecwd
#define PATH_SEPARATOR "/"

#include "commands/command.h"
#include "commands/make_project.h"
#include "utils.h"

int make_project(char **args) {
	d_showInput("make_project", args);
	
	// If there is an argument
	if (args[2] != NULL) {
		e_fatal("'make_project' command does not support arguments\n");
		return 1;
	}
	
	// Check if name is provided
	if (strcmp(g_vals[1][0], "") == 0) {
		e_fatal("name is required to create a project\n");
		return 1;
	}

	char *name = g_vals[1][0];

	char *userpath = NULL;
	// Check if adess should use a path defined by the user
	if (strcmp(g_vals[3][0], "") != 0) {
		userpath = g_vals[3][0];
	}

	char *path = getCurDirectory(userpath);

	// If inside an adess project
	if (checkFileExistsIn(path, "*.adess")) { // search for anything that contains '.adess'
		e_fatal("already inside adess project\n");
		return 1;
	}

	path = strcat(path, name);

	// If making the project directory fails, abort
	if (makeDirectory(path)) {
		return 1;
	}

	char projectFilePath[4096];
	snprintf(projectFilePath, 4096, "%s%s%s%s", path, PATH_SEPARATOR, name, ".adess");

	if (makeFile(projectFilePath) == 1) return 1;
	
	// Exit if option -e (empty) is enabled
	if (g_opts[5] == true) {
		n_print("created empty project '%s' at '%s'\n", name, path);
		return 0;
	}

	// Make 'engines', 'scenes' and 'output' directory
	char enginePath[4096], scenePath[4096], outputPath[4096];
	
	// Set correct directories and add them to the 'project.adess' file
	snprintf(enginePath, 4096, "%s%s%s", path, PATH_SEPARATOR, "engines");
	snprintf(scenePath, 4096, "%s%s%s", path, PATH_SEPARATOR, "scenes");
	snprintf(outputPath, 4096, "%s%s%s", path, PATH_SEPARATOR, "output");

	// Make the directories
	makeDirectory(enginePath);
	makeDirectory(scenePath);
	makeDirectory(outputPath);

	// Append base config into the <name>.adess file
	appendProjectDefault(projectFilePath);

	n_print("created project '%s' at '%s'\n", name, path);
	
	return 0;
}

void appendProjectDefault(char *path) {
	// Define audio signal parameters
	if (appendLine(path, "// Audio signal parameters\n") == 1) return;
	if (appendLine(path, "sample_rate = int: sample rate in Hz (usually 44100 or 96000)\n") == 1) return;
	if (appendLine(path, "bit_depth = int: bit depth of audio: 8, 16, 24 or 32\n") == 1) return;

	// Define adess parameters
	if (appendLine(path, "\n// Paths to files\n") == 1) return;
	if (appendLine(path, "engine_path = string: path to engine files\n") == 1) return;
	if (appendLine(path, "scene_path = string: path to scene files\n") == 1) return;
	if (appendLine(path, "output_path = string: path to output\n") == 1) return;
	if (appendLine(path, "output_path = string: path to output\n") == 1) return;

	if (appendLine(path, "\n") == 1) return;

	if (appendLine(path, "seed = unsigned 32-bit int: seed for random number generation\n") == 1) return;
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

	d_print("directory created at '%s'\n", path);
	
	return 0;
}

