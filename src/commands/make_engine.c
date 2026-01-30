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
		if (appendLine(completePath, "stroke = int: stroke\n") == 1) return 1;
		if (appendLine(completePath, "cylinder_count = int: number of cylinders\n") == 1) return 1;
		if (appendLine(completePath, "idle_rpm = int: rpm at idle\n") == 1) return 1;
		if (appendLine(completePath, "max_rpm = int: maximum rpm\n") == 1) return 1;
		if (appendLine(completePath, "valvetrain_timing_offset = float: offset of exhaust exhaust valves\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Low frequency noise parameters\n") == 1) return 1;
		if (appendLine(completePath, "low_frequency_noise_frequency = float: frequency of low frequency noise\n") == 1) return 1;
		if (appendLine(completePath, "low_frequency_noise_falloff = int: falloff of low frequency noise in rpm\n") == 1) return 1;
		if (appendLine(completePath, "low_frequency_noise_strength = float: strength of low frequency noise\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Harmonics\n") == 1) return 1;
		if (appendLine(completePath, "harmonics = int: number of harmonics\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Volume parameters\n") == 1) return 1;
		if (appendLine(completePath, "base_volume = float: volume of the base layer\n") == 1) return 1;
		if (appendLine(completePath, "valvetrain_volume = float: volume of the valvetrain layer\n") == 1) return 1;
		if (appendLine(completePath, "mechanical_volume = float: volume of the mechanical layer\n") == 1) return 1;
		if (appendLine(completePath, "vibration_volume = float: volume of the vibration layer\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Volume modification parameters\n") == 1) return 1;
		if (appendLine(completePath, "minimum_volume = float: minimum volume (at idle rpm and no load)\n") == 1) return 1;
		if (appendLine(completePath, "rpm_volume_multiplier = float: rpm volume multiplication factor\n") == 1) return 1;
		if (appendLine(completePath, "load_volume_multiplier = float: load volume multiplication factor\n") == 1) return 1;

		if (appendLine(completePath, "\n") == 1) return 1;

		if (appendLine(completePath, "// Noise modification parameters\n") == 1) return 1;
		if (appendLine(completePath, "minimum_noise = float: minimum noise (at idle rpm and no load)\n") == 1) return 1;
		if (appendLine(completePath, "load_noise_multiplier = float: load noise multiplication factor\n") == 1) return 1;

		n_print("created engine at '%s'\n", completePath);
	}

	free(completePath);
	return 0;
}
