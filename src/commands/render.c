#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "commands/command.h"
#include "commands/render.h"
#include "utils.h"

int render(char **args) {
	d_showInput("render", args);
	
	// Check if inside adess project
	if (!checkFileExistsIn(getCurDirectory(NULL), "*.adess")) {
		e_fatal("command 'render' can only be ran inside an adess project directory\n");
		return 1;
	}

	// Check if there is no argument or the -a flag
	if (args[2] == NULL && g_opts[6] == false) {
		e_fatal("no scene to render, add it's name as an argument or use '-a' to render all\n");
		return 1;
	}
	
	// Generate the path to the project file
	char projectFilePath[4096];
	char *tempProjFile = findProjectFile(getCurDirectory(NULL));
	if (tempProjFile == NULL) {
		return 1;
	}

	strncpy(projectFilePath, tempProjFile, 4096);

	// Check if the adess DST file has valid syntax
	if (checkValidity(projectFilePath) == false) {
		return 1;
	}
	
	// If '-a' option is on, render every scene
	if (g_opts[6] == true) {
		return renderAll(projectFilePath);
	} else {
		return renderScene(args[2], projectFilePath);
	}
}

// This whole function is incredibly disgusting, unsafe and sinful
int renderScene(char *rawscenename, char *projectFilePath) {

	// Get scene path

	// Get the path of the requested scene
	char *subpath = parseLineValueS("scene_path", projectFilePath);

	char *scenename = rawscenename;
	// Create the scenename variable with correct data
	if (strcmp(rawscenename + strlen(rawscenename) - 6, ".adess") != 0) {
		sprintf(scenename, "%s.adess", scenename);
	}

	char sceneFilePath[4096];
	if (subpath == NULL) {
		strncpy(sceneFilePath, getCurDirectory(NULL), 4096);
	} else {
		strncpy(sceneFilePath, getCurDirectory(subpath), 4096);
	}

	// Check if scene directory exists
	if (!checkFileExists(sceneFilePath)) {
		e_fatal("scene directory does not exist at '%s'\n", sceneFilePath);
		return 1;
	}
	
	// Check if scene exists
	if (!checkFileExistsIn(sceneFilePath, scenename)) {
		e_fatal("scene '%s' does not exist in directory '%s'\n", scenename, sceneFilePath);
		return 1;
	}

	// Get engine path
	sprintf(sceneFilePath, "%s%s", sceneFilePath, scenename);

	// Check if scene is valid
	if (checkValidity(sceneFilePath) == false) {
		return 1;
	}

	// Get the path of the engine directory
	char engineFilePath[4096];
	strncpy(engineFilePath, getCurDirectory(NULL), 4096);
	subpath = parseLineValueS("engine_path", projectFilePath);

	// Unspecified engine directory = use current directory
	if (subpath != NULL) {
		strncpy(engineFilePath, getCurDirectory(subpath), 4096);
	}

	// Check if the engine directory exists
	

	// Get engine file from scene file
	char *enginename = parseLineValueS("engine", sceneFilePath);
	printf("enginename 1: %s\n", enginename);

	if (enginename == NULL) {
		return 1;
	}
		
	sprintf(enginename, "%s.adess", enginename);

	// Check if engine directory exists
	printf("enginename 2: %s\n", enginename);

	// WTF - strange workaround because 'checkFileExistsIn' removes the enginename for some reason
	char tempenginename[64];
	strcpy(tempenginename, enginename);
	printf("temp: %p\t%p\n", tempenginename, enginename);

	// Check if engine exists
	if (!checkFileExistsIn(engineFilePath, tempenginename)) {
		strcpy(enginename, tempenginename);
		e_fatal("engine '%s' does not exist in directory '%s'\n", enginename, engineFilePath);
		return 1;
	}

	// Get the engine file
	snprintf(engineFilePath, strlen(engineFilePath) + strlen(enginename) + 7, "%s%s.adess", engineFilePath, enginename);

	printf("engineFilePath: %s\n", engineFilePath);

	n_print("scene '%s' rendered successfully into '%s'\n", scenename, "<output_dir>"); // TODO
	return 0;
}

int renderAll(char *projectFilePath) {
	printf("rendering all, %s\n", projectFilePath);
	return 1;
}
