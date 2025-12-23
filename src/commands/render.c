#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <inttypes.h>

#include "commands/command.h"
#include "commands/render.h"
#include "utils.h"
#include "render/render-general.h"
#include "files/export.h"

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
	// This removes enginePathInput, not anymore B)
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

	// Get the output path - the file name will be the same as the scene name, just get the directory
	char *outputPath = parseLineValueS("output_path", projectPath);
	if (outputPath == NULL) {
		return 1;
	}

	outputPath = getCurDirectory(outputPath);
	outputPath = strcat(outputPath, sceneNameInput);

	// This is rather ugly, however, I don't give a -
	outputPath[strlen(outputPath) - 5] = 'w';
	outputPath[strlen(outputPath) - 4] = 'a';
	outputPath[strlen(outputPath) - 3] = 'v';
	outputPath[strlen(outputPath) - 2] = '\0';

	d_print("'output' (%s) found\n", outputPath);

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


	// Sort the keyframes
	if (sortKeys(keyframes, keyframeCount) == false) {
		e_parse(scenePath, getVariableLineNumber("keyframes", scenePath) + 1, "multiple keyframes at the same time\n");
		return 1;
	}

	d_print("Keyframes:\n");
	printKeys(keyframes, keyframeCount);

	// Create main buffer
	float lengthSeconds = parseLineValueF("length", scenePath);
	if (lengthSeconds == FLOAT_FAIL) {
		return 1;
	}
	
	uint32_t sampleRate = parseLineValueI("sample_rate", projectPath);
	if (sampleRate == (uint32_t) INT_FAIL) {
		return 1;
	}

	int64_t sampleCount = lengthSeconds * sampleRate;

	uint16_t bitDepth = parseLineValueI("bit_depth", projectPath);
	if (bitDepth == (uint16_t) INT_FAIL) {
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
		e_fatal("required buffer size is too large [%.2f/%.2f MB], you can override it in '%s/max_buffer_size'\n", (float) bufferSize / 8000000.0f, (float) maxBufferSize / 8589934592.0f, projectPath);
		return 1;
	}

	// Creating the buffer
	uint8_t sampleSize = 0;

	// Set the type to the correct value
	switch (bitDepth) {
	case 8:
		sampleSize = sizeof(uint8_t);
		break;
	case 16:
		sampleSize = sizeof(int16_t);
		break;
	case 24:
		sampleSize = 3 * sizeof(uint8_t); // 24 bit is really weirddd...
		break;
	case 32:
		sampleSize = sizeof(float);
		break;
	}

	void *buffer = (void *) malloc(sampleCount * sampleSize);

	if (buffer == NULL) {
		e_fatal("failed to allocate memory for render buffer of size [%.2f MB]", (sampleCount * sampleSize) / 8000000.0f);
		return 1;
	}

	d_print("Created rpm buffer with [%" PRId64 "] samples of size [%.2f MB] with sample size of [%i bits]\n", sampleCount, (sampleCount *  sampleSize)/ 8000000.0f, bitDepth);

	// Interpolate keyframes (linear)
	keysToSine(buffer, bitDepth, keyframes, keyframeCount, sampleCount, sampleRate);

	free(keyframes);

	// Now write into a WAV file
	
	FILE *file = fopen(outputPath, "wb");

	if (file == NULL) {
		fclose(file);
		e_fatal("failed to write into file '%s'\n", outputPath);
		return 1;
	}

	makeWavHeader(file, sampleRate, bitDepth, (uint32_t) sampleCount);

	fwrite(buffer, 1, sampleCount * (bitDepth / 8), file);

	free(buffer);
	fclose(file);

	return 0;
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

/* TODO move to 'render-general' and make it generic
void keysToSine(int16_t *buffer, uint64_t length, struct Keyframe *keyframes, int keyframeCount, int sampleRate) {
	uint64_t i = 0;

	// If there is only one keyframe
	if (keyframeCount == 1) {
		while (i < length) {
			buffer[i] = keyframes[0].rpm;
			i++;
		}
	}

	// If there are more keyframes

	// Keyframe stuff
	uint16_t lastKey, nextKey = 0;
	float currentTime = 0.0f;

	float lastTime, nextTime = 0.0f;
	uint16_t lastRpm, nextRpm = 0;

	while (i < length) {
		currentTime = (float) i / (float) sampleRate;

		// Calculate the next chronological keyframe
		while (nextKey < keyframeCount) {
			if (keyframes[nextKey].keytime > currentTime) {
				break;
			}
			nextKey++;
		}

		lastKey = nextKey - 1;

		
		// If the next key is also the first key
		// This is GENIUS because it has no drawbacks (I have the exact same amount of maximum keyframes)
		if (lastKey == (uint16_t) -1) {
			lastTime = 0;
			lastRpm = keyframes[nextKey].rpm; // Set to the 'nextKey' rpm
		} else {
			lastTime = keyframes[lastKey].keytime;
			lastRpm = keyframes[lastKey].rpm;
		}

		// Do the same with the next keyframe
		if (nextKey >= keyframeCount) {
			nextTime = (float) length / (float) sampleRate;
			nextRpm = keyframes[lastKey].rpm;
		} else {
			nextTime = keyframes[nextKey].keytime;
			nextRpm = keyframes[nextKey].rpm;
		}

		buffer[i] = lastRpm + (currentTime - lastTime) * ((nextRpm - lastRpm) / (nextTime - lastTime));

		i++;
	}
}
*/

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

/* TODO move to 'render-general.c' and make it generic
void printEveryNBuffer(const uint16_t *buffer, int n, uint64_t length) {
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
*/
