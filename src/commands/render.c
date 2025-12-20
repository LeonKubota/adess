#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <inttypes.h>

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

	// Initilize 'keyframes'
	for (int i = 0; i < keyframeCount; i++) {
		keyframes[i].keytime = 0.0f;
		keyframes[i].rpm = 0;
		keyframes[i].load = 0.0f;
	}

	loadKeyframes(scenePath, keyframes, keyframeCount);

	d_print("Loaded keyframes [%i]:\n", keyframeCount);
	printKeys(keyframes, keyframeCount);

	// Sort the keyframes
	if (sortKeys(keyframes, keyframeCount) == false) {
		e_parse(scenePath, getVariableLineNumber("keyframes", scenePath) + 1, "multiple keyframes at the same time\n");
		return 1;
	}

	d_print("Sorted keyframes [%i]:\n", keyframeCount);
	printKeys(keyframes, keyframeCount);

	// Create main buffer
	float lengthSeconds = parseLineValueF("length", scenePath);
	if (lengthSeconds == FLOAT_FAIL) {
		return 1;
	}
	
	int64_t sampleRate = parseLineValueI("sample_rate", projectPath);
	if (sampleRate == INT_FAIL) {
		return 1;
	}

	int64_t sampleCount = lengthSeconds * sampleRate;

	int bitDepth = parseLineValueI("bit_depth", projectPath);
	if (bitDepth == INT_FAIL) {
		return 1;
	} else if (bitDepth%8 != 0 || bitDepth > 32 || bitDepth == 0) {
		e_parse(projectPath, getVariableLineNumber("bit_depth", projectPath) + 1, "incorrect value for 'bit_depth'\n");
		return 1;
	}

	uint64_t bufferSize = (uint64_t) sampleCount * (uint64_t) bitDepth; // So that it can hold it

	int64_t maxBufferSize = parseLineValueI("max_buffer_size", projectPath);

	if (maxBufferSize == INT_FAIL) {
		return 1;
	}

	if (bufferSize > (uint64_t) maxBufferSize) {
		e_fatal("required buffer size is too large [%.2f/%.2f GB], you can override it in '%s/max_buffer_size'\n", (float) bufferSize / 8589934592.0f, (float) maxBufferSize / 8589934592.0f, projectPath);
		return 1;
	}

	// Creating the buffer
	/* This will be useful when I need add different bit depths TODO
	switch (bitDepth) {
	case 8:
		uint8_t *buffer8 = (uint8_t *) malloc(sampleCount * sizeof(uint8_t));
		break;
	case 16:
		int16_t *buffer16 = (int16_t *) malloc(sampleCount * sizeof(int16_t));
		break;
	case 24:
		int32_t *buffer24 = (int32_t *) malloc(sampleCount * sizeof(int32_t));
		break;
	case 32:
		float *buffer32 = (float *) malloc(sampleCount * sizeof(float));
		break;
	}
	*/
	int16_t *buffer = (int16_t *) malloc(sampleCount * sizeof(int16_t));

	d_print("created buffer with %" PRId64 " samples of size [%.4f GB]\n", sampleCount, (sampleCount * sizeof(int16_t)) / 8589934592.0f);

	// Interpolate keyframes
	interpolateKeys(buffer, sampleCount, keyframes);

	printEveryNBuffer(buffer, 4096*100, sampleCount);
	free(buffer);
	free(keyframes);
	return 1;
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

void interpolateKeys(int16_t *buffer, uint64_t length, struct Keyframe *keyframes) {
	uint64_t i = 0;

	while (i < length) {
		buffer[i] = keyframes[0].rpm;
		i++;
	}
	return;
}

// Bubble sort - true = success
bool sortKeys(struct Keyframe *keyframes, int keyCount) {
	// Check if there are multiple keyframes at the same time
	for (int i = 0; i < keyCount; i++) {
		for (int n = 0; n < keyCount; n++) {
			if (keyframes[i].keytime == keyframes[n].keytime && i != n) {
				return false;
			}
		}
	}

	bool sorted = false;
	struct Keyframe swap;

	for (int i = 0; i < keyCount; i++) {
		sorted = false; // TEMP idk if this is needed

		for (int n = 0; n < keyCount - i - 1; n++) {
			if (keyframes[n].keytime > keyframes[n + 1].keytime) {
				swap = keyframes[n];
				keyframes[n] = keyframes[n + 1];
				keyframes[n + 1] = swap;
				sorted = false;
			}
		}

		if (sorted == true) {
			break;
		}
	}

	return true;
}

void printKeys(struct Keyframe *keyframesPrint, int keyCount) {
	int i = 0;
	if (g_debug == false) {
		return;
	}

	while (i < keyCount) {
		printf("\x1b[1;35m" "[DEBUG]" "\033[m" "\tindex: '%i'; \tkeytime: '%f'; \trpm: '%i'; \tload: '%f';\n", i, keyframesPrint[i].keytime, keyframesPrint[i].rpm, keyframesPrint[i].load);
		i++;
	}
}

void printEveryNBuffer(const int16_t *buffer, int n, uint64_t length) {
	uint64_t i = 0;
	while (i < length) {
		printf("%i\t", buffer[i]);
		i += n;
		if (i%(n*12) == 0) {
			printf("\n");
		}
	}
	printf("\n");
}
