#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>

#include <complex.h> // For complex numbers
#include <pthread.h> // For multithreading

#include <time.h> // For debugging

#include "main.h"
#include "commands/command.h"
#include "commands/render.h"
#include "utils.h"
#include "render/multithreading.h"
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

	static char name[1024];
	name[0] = '\0';

	// Check if the user specified a name
	if (g_opts[1] == true) { // If -n
		if (g_opts[6] == true) {
			e_fatal("can not specify name for multiple scenes\n");
			return 1;
		}

		strcpy(name, processName(g_vals[1][0]));

		if (name[0] == '\0') {
			return 1;
		}
	}
	
	// Generate the path to the project file
	char projectFilePath[4096];
	char *tempProjFile = findProjectFile(getCurDirectory(NULL));
	if (tempProjFile == NULL) return 1;

	strcpy(projectFilePath, tempProjFile);

	// Check if the adess DST file has valid syntax
	if (checkValidity(projectFilePath) == false) return 1;

	// Put things into the project struct
	struct Project *project = (struct Project *) malloc(sizeof(struct Project));
	if (project == NULL) return 1;

	if (getProject(project, projectFilePath) == 1) return 1;
	
	// If '-a' option is on, render every scene
	if (g_opts[6] == true) {
		return renderAll(project);
	} else {
		return renderScene(args[2], project, name);
	}
}

// This function is extremly chaotic
// TODO making it nice and clean (just hiding the ugliness with abstractions)
int renderScene(char *sceneNameInput, struct Project *project, char *name) {
 	time_t startTime = clock();
	// Create scene struct
	struct Scene *scene = (struct Scene *) malloc(sizeof(struct Scene));
	if (scene == NULL) return 1; // Verify creation of struct 'scene'

	// Get scene information into the struct
	if (getScene(scene, sceneNameInput, project) == 1) return 1;


	// Get engine path and validate
	struct Engine *engine = (struct Engine *) malloc(sizeof(struct Engine));
	if (engine == NULL) return 1; // Verify creation of struct 'engine'

	// Get engine information into the struct
	if (getEngine(scene, engine, project) == 1) return 1;


	// Get output path and validate
	char *outputPath = getOutputPath(name, sceneNameInput, project);
	if (outputPath == NULL) return 1;


	// Get keyframe information, sort and precalculate frequency
	struct Keyframe *keyframes = (struct Keyframe *) malloc(scene->keyframeCount * sizeof(struct Keyframe));
	if (keyframes == NULL) return 1;

	// Get the keyframes
	if (getKeyframes(keyframes, scene, engine) == 1) return 1;

	d_print("Files for rendering found and loaded\n");


	// Create threads for rendering process
	pthread_t thread1, thread2, thread3, thread4;


	// STAGE 1
	printf("\n");
	d_print("rendering [1/5] - prepare\n");

	// Interpolation: product: 'frequencyBuffer', 'phaseBuffer', 'loadBuffer'
	float *frequencyBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (frequencyBuffer == NULL) return 1;

	double *phaseBuffer = (double *) malloc(scene->sampleCount * sizeof(double));
	if (phaseBuffer == NULL) return 1;

	float *loadBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (loadBuffer == NULL) return 1;

	float *rpmBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (rpmBuffer == NULL) return 1;

	float *lowFrequencyNoiseMultiplierBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (lowFrequencyNoiseMultiplierBuffer == NULL) return 1;

	struct ThreadData interpolationData = {frequencyBuffer, (float *) phaseBuffer, loadBuffer, rpmBuffer, lowFrequencyNoiseMultiplierBuffer, NULL, NULL, project, scene, engine, keyframes, false};

	if (pthread_create(&thread1, NULL, interpolate, (void *) &interpolationData) != 0) return 1;


	// Noise generation
	// Generate pink noise
	float *pinkNoiseBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (pinkNoiseBuffer == NULL) return 1;

	struct ThreadData pinkNoiseGenerationData = {pinkNoiseBuffer, NULL, NULL, NULL, NULL, NULL, NULL, project, scene, engine, NULL, false};

	if (pthread_create(&thread2, NULL, generatePinkNoise, (void *) &pinkNoiseGenerationData) != 0) return 1;

	// Generate brown noise
	float *stableBrownNoiseBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (stableBrownNoiseBuffer == NULL) return 1;

	struct ThreadData stableBrownNoiseGenerationData = {stableBrownNoiseBuffer, NULL, NULL, NULL, NULL, NULL, NULL, project, scene, engine, NULL, false};

	if (pthread_create(&thread3, NULL, generateStableBrownNoise, (void *) &stableBrownNoiseGenerationData) != 0) return 1;

	// Generate low frequency noise
	float *lowFrequencyNoiseBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (lowFrequencyNoiseBuffer == NULL) return 1;

	struct ThreadData lowFrequencyNoiseGenerationData = {lowFrequencyNoiseBuffer, NULL, NULL, NULL, NULL, NULL, NULL, project, scene, engine, NULL, false};

	if (pthread_create(&thread4, NULL, generateLowFrequencyNoise, (void *) &lowFrequencyNoiseGenerationData) != 0) return 1;


	// Join threads from part 1 of rendering
	if (pthread_join(thread1, NULL) != 0) return 1;
	if (pthread_join(thread2, NULL) != 0) return 1;
	if (pthread_join(thread3, NULL) != 0) return 1;
	if (pthread_join(thread4, NULL) != 0) return 1;

	// Test if any failed
	if (interpolationData.failed == true) return 1;
	if (pinkNoiseGenerationData.failed == true) return 1;
	if (stableBrownNoiseGenerationData.failed == true) return 1;
	if (lowFrequencyNoiseGenerationData.failed == true) return 1;

	free(keyframes); // No longer needed after interpolation

	// STAGE 2: compute
	printf("\n");
	d_print("rendering [2/5] - compute\n");

	// Render base frequencies
	float *baseBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (baseBuffer == NULL) return 1;

	struct ThreadData baseRenderingData = {baseBuffer, (float *) phaseBuffer, loadBuffer, frequencyBuffer, lowFrequencyNoiseMultiplierBuffer, lowFrequencyNoiseBuffer, stableBrownNoiseBuffer, project, scene, engine, NULL, false};

	if (pthread_create(&thread1, NULL, renderBase, (void *) &baseRenderingData) != 0) return 1;


	// Render valvetrain
	float *valvetrainBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (valvetrainBuffer == NULL) return 1;

	struct ThreadData valvetrainRenderingData = {valvetrainBuffer, (float *) phaseBuffer, rpmBuffer, frequencyBuffer, pinkNoiseBuffer, lowFrequencyNoiseBuffer, rpmBuffer, project, scene, engine, NULL, false};

	if (pthread_create(&thread2, NULL, renderValvetrain, (void *) &valvetrainRenderingData) != 0) return 1;


	// Render mechanical
	float *mechanicalBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (mechanicalBuffer == NULL) return 1;

	struct ThreadData mechanicalRenderingData = {mechanicalBuffer, frequencyBuffer, loadBuffer, pinkNoiseBuffer, stableBrownNoiseBuffer, lowFrequencyNoiseBuffer, rpmBuffer, project, scene, engine, NULL, false};

	if (pthread_create(&thread3, NULL, renderMechanical, (void *) &mechanicalRenderingData) != 0) return 1;


	// Render vibration
	float *vibrationBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (vibrationBuffer == NULL) return 1;

	struct ThreadData vibrationRenderingData = {vibrationBuffer, frequencyBuffer, loadBuffer, pinkNoiseBuffer, stableBrownNoiseBuffer, lowFrequencyNoiseBuffer, rpmBuffer, project, scene, engine, NULL, false};

	if (pthread_create(&thread4, NULL, renderVibration, (void *) &vibrationRenderingData) != 0) return 1;


	// Join the threads
	if (pthread_join(thread1, NULL) != 0) return 1;
	if (pthread_join(thread2, NULL) != 0) return 1;
	if (pthread_join(thread3, NULL) != 0) return 1;
	if (pthread_join(thread4, NULL) != 0) return 1;

	// Test if any failed
	if (baseRenderingData.failed == true) return 1;
	if (valvetrainRenderingData.failed == true) return 1;
	if (mechanicalRenderingData.failed == true) return 1;
	if (vibrationRenderingData.failed == true) return 1;


	// Free data from prepare stage
	free(frequencyBuffer);
	free(phaseBuffer);
	free(loadBuffer);
	free(rpmBuffer);
	free(lowFrequencyNoiseMultiplierBuffer);

	free(pinkNoiseBuffer);
	//free(stableBrownNoiseBuffer); // TEMP
	free(lowFrequencyNoiseBuffer);

	// Stage: combine
	printf("\n");
	d_print("rendering [3/5] - join\n");

	
	// Combine the buffers
	float *combinedBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (combinedBuffer == NULL) return 1;

	struct ThreadData combineBuffersData = {combinedBuffer, baseBuffer, valvetrainBuffer, mechanicalBuffer, NULL, NULL, NULL, project, scene, engine, NULL, false};

	// Run in main thread
	combineBuffers((void *) &combineBuffersData);

	// Check for errors
	if (combineBuffersData.failed == true) return 1;


	// Stage: post process
	printf("\n");
	d_print("rendering [4/5] - post process\n");


	float *postProcessedBuffer = (float *) malloc(scene->sampleCount * sizeof(float));
	if (postProcessedBuffer == NULL) return 1;

	struct ThreadData postProcessingData = {postProcessedBuffer, combinedBuffer, NULL, NULL, NULL, NULL, NULL, project, scene, engine, NULL, false};

	// Run in main thread
	postProcess((void *) &postProcessingData);

	// Check for errors
	if (postProcessingData.failed == true) return 1;


	free(combinedBuffer);

	printf("\n");

	/*
	free(baseBuffer);
	free(valvetrainBuffer);
	free(mechanicalBuffer);
	free(vibrationBuffer);
	*/

	// TEST
	printMinMax(postProcessedBuffer, scene, project);

	// Stage: write
	printf("\n");
	d_print("rendering [5/5] - write\n");

	b_todo("writing to: '%s'\n", outputPath);

	FILE *file = fopen(outputPath, "wb");
	if (file == NULL) {
		e_fatal("failed to write into file '%s'\n", outputPath);
		return 1;
	}

	project->bitDepth = 32;

	makeWavHeader(file, project->sampleRate, project->bitDepth, (uint32_t) scene->sampleCount);

	fwrite(postProcessedBuffer, 1, scene->sampleCount * (project->bitDepth / 8), file);

	fclose(file);

	d_print("%.2f ms - render finished\n\tspeed: [%.2f s/s]\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC, (scene->length / ((clock() - startTime) * 1000.0f / CLOCKS_PER_SEC)) * 1000.0f);

	free(project);
	free(scene);
	free(engine);

	/*
	//
	* TODO retire this code
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

	//uint64_t bufferSize = (uint64_t) sampleCount * (uint64_t) bitDepth; // So that it can hold it

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

	uint32_t seed = 456123789;

	// Convert the keys to sine
	keysToSine(buffer, bitDepth, keyframes, keyframeCount, sampleCount, sampleRate, &seed);

	free(engine);
	free(keyframes);
	
	b_todo("writing to: '%s'\n", outputPath);

	FILE *file = fopen(outputPath, "wb");

	if (file == NULL) {
		e_fatal("failed to write into file '%s'\n", outputPath);
		return 1;
	}

	makeWavHeader(file, sampleRate, bitDepth, (uint32_t) sampleCount);

	fwrite(buffer, 1, sampleCount * (bitDepth / 8), file);

	free(buffer);
	fclose(file);

	printf("rendered succesfully\n");

	return 0;
	*/
	return 0;
}

int renderAll(struct Project *project) {
	printf("rendering all, %s\n", project->scenePath);
	return 1;
}

int getProject(struct Project *project, char *projectFilePath) {
	char *tempProjFile = findProjectFile(getCurDirectory(NULL));
	if (tempProjFile == NULL) return 1;

	strncpy(projectFilePath, tempProjFile, 1024);

	if (checkValidity(projectFilePath) == false) return 1;

	// Load things into project struct
	project->sampleRate = parseLineValueI("sample_rate", projectFilePath);
	if (project->sampleRate == INT_FAIL) return 1;

	project->bitDepth = parseLineValueI("bit_depth", projectFilePath);
	if (project->bitDepth == INT_FAIL) return 1;

	// Paths
	strcpy(project->scenePath, "\0");
	strcpy(project->enginePath, "\0");
	strcpy(project->outputPath, "\0");

	strcpy(project->scenePath, parseLineValueS("scene_path", projectFilePath));
	if (strcmp(project->scenePath, "\0") == 0) return 1;

	strcpy(project->enginePath, parseLineValueS("engine_path", projectFilePath));
	if (strcmp(project->enginePath, "\0") == 0) return 1;

	strcpy(project->outputPath, parseLineValueS("output_path", projectFilePath));
	if (strcmp(project->outputPath, "\0") == 0) return 1;

	// Seed
	project->seed = parseLineValueI("seed", projectFilePath);
	if (project->seed == INT_FAIL) return 1;

	return 0;
}

int getScene(struct Scene *scene, char *sceneNameInput, struct Project *project) {
	char *scenePathInput = project->scenePath;
	if (scenePathInput == NULL) return 1;

	char *scenePath = getThingPath(scenePathInput, sceneNameInput, "scene");
	if (scenePath == NULL) return 1;

	// Check validity of scene file
	if (checkValidity(scenePath) == false) return 1;

	// Load information into struct
	strcpy(scene->engine, "\0");
	strcpy(scene->engine, parseLineValueS("engine", scenePath));
	if (strcmp(scene->engine, "\0") == 0) return 1;

	scene->length = parseLineValueF("length", scenePath);
	if (scene->length == FLOAT_FAIL) return 1;

	// Non-user defined things
	strcpy(scene->scenePath, scenePath);

	scene->sampleCount = (int) scene->length * project->sampleRate;

	scene->keyframeCount = countKeyframes(scenePath);
	if (scene->keyframeCount == -1) return 1;

	return 0;
}

int getEngine(struct Scene *scene, struct Engine *engine, struct Project *project) {
	char *engineNameInput = parseLineValueS("engine", scene->scenePath);
	if (engineNameInput == NULL) return 1;

	char *enginePath = getThingPath(project->enginePath, engineNameInput, "engine");
	if (enginePath == NULL) return 1;

	// Check validity of engine file
	if (checkValidity(enginePath) == false) return 1;


	// Load information into struct
	// Physical engine parameters
	engine->stroke = parseLineValueI("stroke", enginePath);
	if (engine->stroke == INT_FAIL) return 1;

	if (engine->stroke != 4 && engine->stroke != 2) {
		e_warning("non-standard stroke: [%i]\n", engine->stroke); // ;) a bit freaky
	}

	engine->cylinderCount = parseLineValueI("cylinder_count", enginePath);
	if (engine->cylinderCount == INT_FAIL) return 1;

	engine->idleRpm = parseLineValueI("idle_rpm", enginePath);
	if (engine->idleRpm == INT_FAIL) return 1;

	engine->maxRpm = parseLineValueI("max_rpm", enginePath);
	if (engine->maxRpm == INT_FAIL) return 1;

	engine->valvetrainTimingOffset = parseLineValueF("valvetrain_timing_offset", enginePath);
	if (engine->valvetrainTimingOffset == FLOAT_FAIL) return 1;


	// Harmonics
	engine->harmonics = parseLineValueI("harmonics", enginePath);
	if (engine->harmonics == INT_FAIL) return 1;

	if (engine->harmonics > 30) {
		e_warning("harmonics rarely exceed 30 and are very computationally demanding\n");
	}

	if (engine->harmonics > 254) {
		e_fatal("harmonics can not be over 254\n");
		return 1;
	}
	

	// Noise parameters
	// 'lowFrequencyNoise' settings
	engine->lowFrequencyNoiseFrequency = parseLineValueF("low_frequency_noise_frequency", enginePath);
	if (engine->lowFrequencyNoiseFrequency == FLOAT_FAIL) return 1;

	engine->lowFrequencyNoiseFalloff = parseLineValueI("low_frequency_noise_falloff", enginePath);
	if (engine->lowFrequencyNoiseFalloff == INT_FAIL) return 1;

	engine->lowFrequencyNoiseStrength = parseLineValueF("low_frequency_noise_strength", enginePath);
	if (engine->lowFrequencyNoiseStrength == FLOAT_FAIL) return 1;


	// Volume parameters
	engine->baseVolume = parseLineValueF("base_volume", enginePath);
	if (engine->baseVolume == FLOAT_FAIL) return 1;

	engine->valvetrainVolume = parseLineValueF("valvetrain_volume", enginePath);
	if (engine->valvetrainVolume == FLOAT_FAIL) return 1;

	engine->mechanicalVolume = parseLineValueF("mechanical_volume", enginePath);
	if (engine->mechanicalVolume == FLOAT_FAIL) return 1;

	engine->vibrationVolume = parseLineValueF("vibration_volume", enginePath);
	if (engine->vibrationVolume == FLOAT_FAIL) return 1;

	
	// Volume stuff
	engine->minimumVolume = parseLineValueF("minimum_volume", enginePath);
	if (engine->minimumVolume == FLOAT_FAIL) return 1;

	engine->loadVolumeMultiplier = parseLineValueF("load_volume_multiplier", enginePath);
	if (engine->loadVolumeMultiplier == FLOAT_FAIL) return 1;

	engine->rpmVolumeMultiplier = parseLineValueF("rpm_volume_multiplier", enginePath);
	if (engine->rpmVolumeMultiplier == FLOAT_FAIL) return 1;


	// Noise amount
	engine->minimumNoise = parseLineValueF("minimum_noise", enginePath);
	if (engine->minimumNoise == FLOAT_FAIL) return 1;

	engine->loadNoiseMultiplier = parseLineValueF("load_noise_multiplier", enginePath);
	if (engine->loadNoiseMultiplier == FLOAT_FAIL) return 1;

	return 0;
}

char *getOutputPath(char *name, char *sceneNameInput, struct Project *project) {
	char *outputPath = getCurDirectory(project->outputPath);

	char *outputPathFinal = (char *) malloc(1024 * sizeof(char));

	// Use 'name' if provided, otherwise use sceneName
	if (strlen(name) != 0) {
		strcat(outputPath, name);
		strcpy(outputPathFinal, outputPath);
	} else {
		strcpy(outputPathFinal, outputPath);
		strcat(outputPathFinal, sceneNameInput);

		// Add the .wav extention (and override the '.adess' as a nice bonus) + it's disgusting
		outputPathFinal[strlen(outputPathFinal) - 5] = 'w';
		outputPathFinal[strlen(outputPathFinal) - 4] = 'a';
		outputPathFinal[strlen(outputPathFinal) - 3] = 'v';
		outputPathFinal[strlen(outputPathFinal) - 2] = '\0';
	}

	return outputPathFinal;
}

int getKeyframes(struct Keyframe *keyframes, struct Scene *scene, struct Engine *engine) {
	if (scene->keyframeCount == 0) {
		e_parse(scene->scenePath, 0, "scenes must have at least one keyframe\n");
		return 1;
	}

	for (int i = 0; i < scene->keyframeCount; i++) {
		keyframes[i].keytime = 0.0f;
		keyframes[i].rpm = 0;
		keyframes[i].load = 0.0f;
	}


	if (keyframes == NULL) {
		e_fatal("keyframe buffer creation failed\n");
		return 1;
	}

	loadKeyframes(scene, keyframes); // FIX

	if (sortKeys(keyframes, scene->keyframeCount) == false) {
		e_parse(scene->scenePath, getVariableLineNumber("keyframes", scene->scenePath) + 1, "keyframes must have unique times\n");
		return 1;
	}

	// Precalculate keyframe rpm to frequency (speed it up later)
	rpmToFrequency(keyframes, scene->keyframeCount, engine);

	return 0;
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

void rpmToFrequency(struct Keyframe *keyframes, int keyCount, struct Engine *engine) {
	int i = 0;

	float n = (2.0f / engine->stroke);

	while (i < keyCount) {
		keyframes[i].rpm = keyframes[i].rpm / 60; // Convert to rps
		keyframes[i].rpm = keyframes[i].rpm * n * engine->cylinderCount;
		i++;
	}
}

char *processName(char *inputName) {
	if (strchr(inputName, '/') != NULL) {
		e_fatal("invalid output name, illegal character '/'\n");
		return "\0";
	}
	
	if (strchr(inputName, '.') != NULL) {
		char *wavString = ".wav";
		for (int i = 3; i > -1; i--) {
			printf("%i\n", i);
			if (inputName[strlen(inputName) - i - 1] != wavString[3 - i]) {
				// Doesn't end in ".wav", add the ".wav"
				if (strlen(inputName) > 1020) { // Check if there is space
					e_fatal("invalid output name, too long\n");
					return "\0";
				}

				return strcat(inputName, ".wav");
			}
		}
	} else { // Not even a dot, for sure doesn't end in ".wav"
		if (strlen(inputName) > 1020) { // Check if there is space
			e_fatal("invalid output name, too long\n");
			return "\0";
		}

		return strcat(inputName, ".wav");
	}

	return inputName;
}

void printKeys(struct Keyframe *keyframesPrint, int keyCount) {
	int i = 0;
	if (g_debug == false) {
		return;
	}

	while (i < keyCount) {
		printf("\x1b[1;35m" "[DEBUG]" "\033[m" "\tindex: '%i'; \tkeytime: '%f'; \trpm: '%f'; \tload: '%f';\n", i, keyframesPrint[i].keytime, keyframesPrint[i].rpm, keyframesPrint[i].load);
		i++;
	}
}

void printMinMax(float *buffer, struct Scene *scene, struct Project *project) {
	float maximum = 0.0f;
	float minimum = 0.0f;

	uint64_t maxTime = 0;
	uint64_t minTime = 0;

	uint64_t i = 0;

	while (i < scene->sampleCount) {
		if (buffer[i] > maximum) {
			maximum = buffer[i];
			maxTime = i;
		}

		if (buffer[i] < minimum) {
			minimum = buffer[i];
			minTime = i;
		}

		i++;
	}

	d_print("maximum: [%f at %.2f s]\n", maximum, maxTime / (float) project->sampleRate);
	d_print("minimum: [%f at %.2f s]\n", minimum, minTime / (float) project->sampleRate);
}

uint64_t calculateSmallestPower(uint64_t x) {
	uint64_t i = 0;
	uint64_t n = 2;

	while (n < x) {
		n = 1 << i;
		i++;
	}

	return n;
}
