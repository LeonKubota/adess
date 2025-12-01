#include <stdio.h>
#include <string.h>

#include "commands/command.h"
#include "commands/render.h"
#include "utils.h"

int render(char **args) {
	d_showInput("render", args);
	
	// Check if inside adess project
	if (!checkFileExistsIn(getCurDirectory(NULL), ".adess")) {
		e_fatal("command 'render' can only be ran inside an adess project directory\n");
		return 1;
	}

	// Check if there is no argument or the -a flag
	if (args[2] == NULL && g_opts[6] == false) {
		e_fatal("no scene to render, add it's name as an argument or use '-a' to render all\n");
		return 1;
	}
	
	// Generate the path to the project file
	char *projectFilePath = getCurDirectory(NULL);
	projectFilePath = findProjectFile(projectFilePath);

	// Check if the adess DST file has valid syntax
	if (checkValidity(projectFilePath) == false) {
		return 1;
	}
	
	// If '-a' option is on, render every scene
	if (g_opts[6] == true) {
		return renderAll(projectFilePath);
	} else {
		return renderScene(args, projectFilePath);
	}

	return 0;
}

int renderAll(char *projectFilePath) {
	printf("rendering all");
	return 1;
}

int renderScene(char **sceneNames, char *projectFilePath) {
	// Get the path of the requested scene
	//char *subpath = parseLineValueS("scene_path", projectFilePath);
	return 0;

	// Unspecified scene directory = use current directory
	/*
	if (subpath == NULL) {
		if (checkFileExistsIn(projectFilePath, sceneNames[0]) {
			

	// Get the path of the requested engine file
	char *engineFilePath = getCurDirectory(NULL);
	subpath = parseLineValueS("engine_path", projectFilePath);

	// Unspecified engine directory - use current directory
	if (subpath == NULL) {
		
	}
	*/
}
