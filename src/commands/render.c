#include <stdio.h>
#include <string.h>

#include "commands/command.h"
#include "commands/render.h"
#include "utils.h"

int render(char *arg) {
	d_showInput("render", arg);
	
	// Check if inside adess project
	if (!checkFileExistsIn(getCurDirectory(NULL), ".adess")) {
		e_fatal("command 'render' can only be ran inside an adess project directory\n");
		return 1;
	}

	// Check if there is no argument or the -a flag
	if (arg == NULL && g_opts[6] == false) {
		e_fatal("no scene to render, add it's name as an argument or use '-a' to render all\n");
		return 1;
	}
	
	if (g_opts[6] == true) {
		return renderAll();
	}
	
	// Generate the path to the project file
	char *projectFilePath = getCurDirectory(NULL);
	projectFilePath = findProjectFile(projectFilePath);
	
	printf("engine_path: %s\n", parseLineValueS("engine_path", projectFilePath));
	printf("max_buffer_size: %i\n", parseLineValueI("max_buffer_size", projectFilePath));
	
	return 0;
}

int renderAll() {
	printf("rendering all");
	return 1;
}
