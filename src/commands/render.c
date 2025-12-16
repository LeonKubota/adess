#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h> // For Linux

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
// It doesn't even work now FIXME, this code is complete and utter garbage and needs a refactor ASAP

// Ok I think it might be healing
int renderScene(char *inputSceneName, char *projectFilePath) {
	// Get scene path
	char *scenePathInput = parseLineValueS("scene_path", projectFilePath);
	char *scenePath = getScenePath(scenePathInput, inputSceneName);

	// Check if the scene path is real
	if (scenePath == NULL) {
		return 1;
	}

	return 1;
	/*

	// Get engine path
	sceneFilePath = strcat(sceneFilePath, scenename);

	// Check if scene is valid
	if (checkValidity(sceneFilePath) == false) {
		return 1;
	}

	// Get the path of the engine directory
	char *engineFilePath = "";
	strcpy(engineFilePath, getCurDirectory(NULL));
	subpath = parseLineValueS("engine_path", projectFilePath);

	// Unspecified engine directory = use current directory
	if (subpath != NULL) {
		strcpy(engineFilePath, getCurDirectory(subpath));
	}

	// Check if the engine directory exists
	

	// Get engine file from scene file
	char *enginename = parseLineValueS("engine", sceneFilePath);

	// If there is an engine
	if (enginename == NULL) {
		return 1;
	}

	if (enginename == NULL) {
		return 1;
	}
		
	enginename = strcat(enginename, ".adess");

	// WTF - strange workaround because 'checkFileExistsIn' removes the enginename for some reason
	char tempenginename[64];
	strcpy(tempenginename, enginename);

	// Check if engine exists
	if (!checkFileExistsIn(engineFilePath, tempenginename)) {
		strcpy(enginename, tempenginename);
		e_fatal("engine '%s' does not exist in directory '%s'\n", enginename, engineFilePath);
		return 1;
	}

	// Get the engine file
	engineFilePath = strncat(engineFilePath, strcat(enginename, ".adess"), strlen(engineFilePath) + strlen(enginename));
	//snprintf(engineFilePath, strlen(engineFilePath) + strlen(enginename) + 7, "%s%s.adess", engineFilePath, enginename);

	// Now do the actual rendering stuff 

	// Create the buffer
	
	float lengthseconds = parseLineValueF("length", sceneFilePath);
	if (lengthseconds == FLOAT_FAIL) {
		return 1;
	}

	int samplerate = parseLineValueI("sample_rate", projectFilePath);
	if (samplerate == INT_FAIL) {
		return 1;
	}

	int buffersize = lengthseconds * samplerate;

	int maxbuffersize = parseLineValueI("max_buffer_size", projectFilePath);
	if (maxbuffersize == INT_FAIL) {
		return 1;
	}

	if (buffersize > maxbuffersize) {
		e_fatal("required buffer size is too big, you can override it in '%s/max_buffer_size'\n", projectFilePath);
		return 1;
	}

	// Create the buffer
	int *buffer = (int *)malloc(buffersize * sizeof(int));

	float time[4] = {0.15f, 0.30f, 0.60f, 1.25f};
	int rpm[4] = {1000, 1100, 2000, 3000};
	float load[4] = {0.0f, 0.1f, 0.1f, 0.2f};

	// Load in keyframes
	interpolateKeys(buffer, buffersize, time, rpm, load);

	int i = 0;
	while (i < buffersize) {
		printf("%i", buffer[i]);
		i++;
	}

	// TEMP used so it takes a bit so I can get the amount of used memory
	sleep(5);

	n_print("scene '%s' rendered successfully into '%s'\n", scenename, "<output_dir>"); // TODO
	free(buffer);
	return 0;
	*/
}

int renderAll(char *projectFilePath) {
	printf("rendering all, %s\n", projectFilePath);
	return 1;
}

// Helper functions for render function, in execution order
char *getScenePath(char *scenePath, char *sceneName) {
	// Prepare the sceneName
	char *processedSceneName = strcat(sceneName, ".adess");

	// Get the directory path
	char *processedScenePath;
	processedScenePath = getCurDirectory(scenePath);

	// Check if it's a real directory or if the user is stupid
	if (!checkFileExists(processedScenePath)) {
		e_fatal("scene directory does not exist at '%s'\n", processedScenePath);
		return NULL;
	}

	// Make the complete path (path + name)
	char *completePath = (char *)malloc(strlen(processedScenePath) + strlen(processedSceneName) + 1); // +1 for terminator ("I'll be back" - Terminator)
	strcpy(completePath, processedScenePath);
	strcat(completePath, processedSceneName);

	// Check if the scene exists in the directory
	if (checkFileExists(completePath) == true) {
		return completePath;
	} else {
		e_fatal("scene '%s' does not exist in '%s'\n", sceneName, processedScenePath);
		return NULL;
	}


	free(completePath);
	return NULL;
}

void interpolateKeys(int *bufferkey, int buffersizekey, float *time, int *rpm, float *load) {
	printf("time: %f\n", time[0]);
	printf("rpm: %i\n", rpm[0]);
	printf("load: %f\n", load[0]);
	int i = 0;
	while (i <= buffersizekey) {
		bufferkey[i] = 1;
		i++;
	}
	return;
}
