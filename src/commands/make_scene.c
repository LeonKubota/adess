#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "commands/command.h"
#include "commands/make_scene.h"
#include "utils.h"
#include "files/check.h"

#define PATH_SEPARATOR "/"

int make_scene(char **args) {
	d_showInput("make_scene", args);

	// If there is an argument
	if (args[2] != NULL) {
		e_fatal("'make_scene' command does not support arguments\n");
		return 1;
	}

	// Check if there is a name
	if (strcmp(g_vals[1][0], "") == 0) {
		e_fatal("name is required to create a scene\n");
		return 1;
	}

	// Check if the name is valid
	uint16_t i = 0;
	while (true) {
		// Check for '/'
		if (g_vals[1][0][i] == '/') {
			e_fatal("name '%s' is not valid, contains character '/'\n", g_vals[1][0]);
			return 1;
		}

		// Exit the loop
		if (g_vals[1][0][i] == '\0') break;

		i++;
	}

	char *userpath = NULL;
	// Check if adess should use a path defined by the user
 	if (strcmp(g_vals[3][0], "") != 0) {
		userpath = g_vals[3][0];
	}

	char *path = getCurDirectory(userpath);
	if (path == NULL) return 1;

	// If inside an adess project
	if (!checkFileExistsIn(path, "*.adess")) { // search for anything that contains '.adess'
		e_fatal("scenes can only be created inside an adess project\n");
		return 1;
	}

	char *projectPath = findProjectFile(getCurDirectory(NULL));
	if (projectPath == NULL) return 1;


	// Get the 'scene_path' string defined by the user
	char *scenePath = parseLineValueS("scene_path", projectPath);
	if (scenePath == NULL) return 1;

	free(projectPath);


	char *name = g_vals[1][0];
	free(path);
	path = getCurDirectory(NULL);


	// Create complete path
	char *completePath = (char *) malloc((strlen(path) + strlen(scenePath) + strlen(PATH_SEPARATOR) + strlen(name) + strlen(".adess") + 1) * sizeof(char));

	strcpy(completePath, path);
	free(path);

	strcat(completePath, scenePath);
	free(scenePath);

	strcat(completePath, PATH_SEPARATOR);
	strcat(completePath, name);
	strcat(completePath, ".adess");


	if (makeFile(completePath) == 1) {
		free(completePath);
		return 1;
	}

	// Append information
	if (g_opts[5] == true) {
		n_print("created empty scene at '%s'\n", completePath);
	} else {
		if (appendLine(completePath, "length = 10.0f\t\t\t//float: length in seconds\n") == 1) return 1;
		if (appendLine(completePath, "engine = \"engineName\"\t//string: engine_name\n") == 1) return 1;

		if (appendLine(completePath, "\nkeyframes = {\n") == 1) return 1;
		if (appendLine(completePath, "\t0.0f, 1000, 0.0f;\t//float: time in seconds, int: rpm, float: load (0 to 1)\n") == 1) return 1;
		if (appendLine(completePath, "}\n") == 1) return 1;

		n_print("created scene at '%s'\n", completePath);
	}

	free(completePath);
	return 0;
}
