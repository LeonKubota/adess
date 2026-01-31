#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "commands/command.h"
#include "commands/make_engine.h"
#include "utils.h"
#include "files/check.h"

#define PATH_SEPARATOR "/"

int make_engine(char **args) {
	d_showInput("make_engine", args);

	// If there is an argument
	if (args[2] != NULL) {
		e_fatal("'make_engine' command does not support arguments\n");
		return 1;
	}

	// Check if there is a name
	if (strcmp(g_vals[1][0], "") == 0) {
		e_fatal("name is required to create an engine\n");
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
		e_fatal("engines can only be created inside an adess project\n");
		return 1;
	}

	char *projectPath = findProjectFile(getCurDirectory(NULL));
	if (projectPath == NULL) return 1;


	// Get the 'engine_path' string defined by the user
	char *scenePath = parseLineValueS("engine_path", projectPath);
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
		n_print("created empty engine at '%s'\n", completePath);
	} else {
		// Physical engine parameters
		if (appendLine(completePath, "// Physical engine parameters\n") == 1) return 1;
		if (appendLine(completePath, "stroke = 4\t\t\t\t\t\t\t\t//int: stroke\n") == 1) return 1;
		if (appendLine(completePath, "cylinder_count = 4\t\t\t\t\t\t//int: number of cylinders\n") == 1) return 1;
		if (appendLine(completePath, "idle_rpm = 1000\t\t\t\t\t\t\t//int: rpm at idle\n") == 1) return 1;
		if (appendLine(completePath, "max_rpm = 6000\t\t\t\t\t\t\t//int: maximum rpm\n") == 1) return 1;
		if (appendLine(completePath, "valvetrain_timing_offset = 1.5f\t\t\t//float: offset of exhaust exhaust valves\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Low frequency noise parameters\n") == 1) return 1;
		if (appendLine(completePath, "low_frequency_noise_frequency = 15.0f\t//float: frequency of low frequency noise\n") == 1) return 1;
		if (appendLine(completePath, "low_frequency_noise_falloff = 100\t\t//int: falloff of low frequency noise in rpm\n") == 1) return 1;
		if (appendLine(completePath, "low_frequency_noise_strength = 0.5f\t\t//float: strength of low frequency noise\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Harmonics\n") == 1) return 1;
		if (appendLine(completePath, "harmonics = 3\t\t\t\t\t\t\t//int: number of harmonics\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Volume parameters\n") == 1) return 1;
		if (appendLine(completePath, "base_volume = 1.0f\t\t\t\t\t\t//float: volume of the base layer\n") == 1) return 1;
		if (appendLine(completePath, "valvetrain_volume = 0.05f\t\t\t\t//float: volume of the valvetrain layer\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Volume modification parameters\n") == 1) return 1;
		if (appendLine(completePath, "minimum_volume = 0.3f\t\t\t\t\t//float: minimum volume (at idle rpm and no load)\n") == 1) return 1;
		if (appendLine(completePath, "rpm_volume_multiplier = 0.4f\t\t\t//float: rpm volume multiplication factor\n") == 1) return 1;
		if (appendLine(completePath, "load_volume_multiplier = 0.1f\t\t\t//float: load volume multiplication factor\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Noise modification parameters\n") == 1) return 1;
		if (appendLine(completePath, "minimum_noise = 0.3f\t\t\t\t\t//float: minimum noise (at idle rpm and no load)\n") == 1) return 1;
		if (appendLine(completePath, "load_noise_multiplier = 0.5f\t\t\t//float: load noise multiplication factor\n") == 1) return 1;

		n_print("created engine at '%s'\n", completePath);
	}

	free(completePath);
	return 0;
}
