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
int renderScene(char *sceneNameInput, char *projectPath) {
	// Get scene path
	char *scenePathInput = parseLineValueS("scene_path", projectPath);
	if (scenePathInput == NULL) {
		return 1;
	}

	char *scenePath = getThingPath(scenePathInput, sceneNameInput, "scene");
	if (scenePath == NULL) {
		return 1;
	}

	// Check validity of scene file
	if (checkValidity(scenePath) == false) {
		return 1;
	}

	d_print("'scene_path' (%s) found\n", scenePath);

	// Get engine path
	char enginePathInput[4096];
	strcpy(enginePathInput, parseLineValueS("engine_path", projectPath));
	if (enginePathInput[0] == '\0') {
		return 1;
	}

	// Get engine name
	// This removes enginePathInput, not anymore B-)
	char *engineNameInput = parseLineValueS("engine", scenePath);
	if(engineNameInput == NULL) {
		return 1;
	}

	char *enginePath = getThingPath(enginePathInput, engineNameInput, "engine");
	if (enginePath == NULL) {
		return 1;
	}

	if (checkValidity(enginePath) == false) {
		return 1;
	}

	d_print("'engine' (%s) found\n", enginePath);

	// Load in keyframes

	// Create the keyframes array
	int keyframeCount = countKeyframes(scenePath);

	if (keyframeCount == 0) {
		e_parse(scenePath, 0, "scenes must have at least one keyframe\n");
		return 1;
	}

	// Could have used 'typedef' but whatever
	struct Keyframe *keyframes = (struct Keyframe *)malloc(keyframeCount * sizeof(struct Keyframe));

	// TEMP
	printf("keytime: %f\n", keyframes[0].keytime);
	
	//loadKeyframes(scenePath);

	return 1;
	/*
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
char *getThingPath(char *thingPath, char *thingName, char *thingType) {
	// Prepare the sceneName
	char *processedName = strcat(thingName, ".adess");

	// Get the directory path
	char *processedPath;
	processedPath = getCurDirectory(thingPath);

	// Check if it's a real directory or if the user is stupid
	if (!checkFileExists(processedPath)) {
		e_fatal("%s directory does not exist at '%s'\n", thingType, processedPath);
		return NULL;
	}

	// Make the complete path (path + name)
	char *completePath = (char *)malloc(strlen(processedPath) + strlen(processedName) + 1); // +1 for terminator ("I'll be back" - Terminator)
	strcpy(completePath, processedPath);
	strcat(completePath, processedName);

	// Check if the scene exists in the directory
	if (checkFileExists(completePath) == true) {
		return completePath;
	} else {
		e_fatal("%s '%s' does not exist in '%s'\n", thingType, thingName, processedPath);
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
