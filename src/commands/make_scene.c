#include "commands/command.h"
#include "utils.h"
#include "commands/make_scene.h"

int make_scene(char **args) {
	d_showInput("make_scene", args);

	// If there is an argument
	if (args[2] != NULL) {
		e_fatal("'make_scene' command does not support arguments\n");
		return 1;
	}

	// Check if 
	if (strcmp(g_vals[1][0], "") == 0) {
		e_fatal("name is required to create a scene\n");
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
	if (!checkFileExistsIn(path, ".adess")) { // search for anything that contains '.adess', kind of like 'grep'
		e_fatal("scenes can only be created inside an adess project\n");
		return 1;
	}

	path = strcat(path, name);
}
