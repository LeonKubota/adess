#include <stdint.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "main.h"
#include "utils.h"
#include "commands/render.h"
#include "render/multithreading.h"

// STAGE 1
// Interpolate keyframes, output 'frequencyBuffer' and 'loadBuffer'
void *interpolate(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	// Get data from threadData
	float *frequencyBuffer = threadData->buffer0;
	double *phaseBuffer = (double *) threadData->buffer1; // A little lie never hurt anyone
	float *loadBuffer = threadData->buffer2;
	float *rpmBuffer = threadData->buffer3;
	float *lowFrequencyNoiseMultiplierBuffer = threadData->buffer4;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Engine *engine = threadData->engine;
	struct Keyframe *keyframes = threadData->keyframes;

	double phase = 0.0f;
	float timeStep = 1.0f / project->sampleRate;

	// For low frequency noise multiplier buffer
	uint32_t lowFrequencyNoiseFalloff = engine->lowFrequencyNoiseFalloff * 16100;
	float lowFrequencyNoiseConstant = (engine->idleRpm * engine->idleRpm) / lowFrequencyNoiseFalloff;

	printf("%f\n", engine->lowFrequencyNoiseStrength);

	uint64_t i = 0;

	// If there is only one keyframe
	if (scene->keyframeCount == 1) {
		// Everything can be precalculated because rpm is constant (except phase)
		rpmBuffer[0] = keyframes[0].rpm * 60.0f / (2.0f / engine->stroke) / engine->cylinderCount;
		lowFrequencyNoiseMultiplierBuffer[0] = engine->lowFrequencyNoiseStrength * (((-(keyframes[0].rpm * keyframes[0].rpm) - (2 * keyframes[0].rpm * engine->idleRpm)) / lowFrequencyNoiseFalloff) - lowFrequencyNoiseConstant + 1.0f);

		while (i < scene->sampleCount) {
			frequencyBuffer[i] = keyframes[0].rpm;
			rpmBuffer[i] = rpmBuffer[0];
			lowFrequencyNoiseMultiplierBuffer[i] = lowFrequencyNoiseMultiplierBuffer[0];

			// Calculate phase
			phase+= TAU * keyframes[0].rpm * timeStep;
			phaseBuffer[i] = phase;

			loadBuffer[i] = keyframes[0].load;

			i++;
		}
	} else {
		uint32_t prevKey, nextKey = 0;

		float prevFrequency, nextFrequency = 0.0f;
		float prevTime, nextTime = 0.0f;
		float prevLoad, nextLoad = 0.0f;

		float currentTime = 0.0f;
		float endTime = (float) scene->sampleCount / project->sampleRate;

		while (i < scene->sampleCount) {
			currentTime = (float) i / project->sampleRate;

			while (nextKey < (uint32_t) scene->keyframeCount) {
				if (keyframes[nextKey].keytime > currentTime) {
					break;
				}
				nextKey++;
			}

			prevKey = nextKey - 1;

			if (prevKey == UINT32_MAX) { // if 'prevKey' equals 2^32 - 1
				prevTime = 0.0f;
				prevFrequency = keyframes[nextKey].rpm;
				prevLoad = keyframes[nextKey].load;
			} else {
				prevTime = keyframes[prevKey].keytime;
				prevFrequency = keyframes[prevKey].rpm;
				prevLoad = keyframes[prevKey].load;
			}

			if (nextKey >= (uint32_t) scene->keyframeCount) {
				nextTime = endTime;
				nextFrequency = keyframes[prevKey].rpm;
				nextLoad = keyframes[prevKey].load;
			} else {
				nextTime = keyframes[nextKey].keytime;
				nextFrequency = keyframes[nextKey].rpm;
				nextLoad = keyframes[nextKey].load;
			}

			// Calculate frequency
			frequencyBuffer[i] = prevFrequency + (currentTime - prevTime) * ((nextFrequency - prevFrequency) / (nextTime - prevTime));

			// Calculate rpm
			rpmBuffer[i] = frequencyBuffer[i] * 60.0f / (2.0f / engine->stroke) / engine->cylinderCount;


			// Calculate low frequency noise multiplier buffer
			// This is the formula: strength * ((( -rpm^2 - 2 * rpm * idle_rpm ) / falloff) - (( idle_rpm^2 ) / falloff) + 1)
			lowFrequencyNoiseMultiplierBuffer[i] = engine->lowFrequencyNoiseStrength * (((-(rpmBuffer[i] * rpmBuffer[i]) - (2 * rpmBuffer[i] * engine->idleRpm)) / lowFrequencyNoiseFalloff) - lowFrequencyNoiseConstant + 1.0f);
			if (lowFrequencyNoiseMultiplierBuffer[i] < 0.0f) lowFrequencyNoiseMultiplierBuffer[i] = 0.0f;

			// Calculate phase
			phase += TAU * frequencyBuffer[i] * timeStep;
			phaseBuffer[i] = phase;

			// Calculate load
			loadBuffer[i] = prevLoad + (currentTime - prevTime) * ((nextLoad - prevLoad) / (nextTime - prevTime));

			i++;
		}
	}

	d_print("%.2f ms - interpolation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *generatePinkNoise(void *arg) {
 	time_t startTime = clock();

	// Test
	d_print("%.2f ms - pink noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC); return NULL;

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *pinkNoiseBuffer = threadData->buffer0;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	uint8_t n = 0;
	float sum = 0.0f;
	float pinkBuffer[8] = {0};

	float randomFloat = 0.0f;

	while (i < scene->sampleCount) {
		// Generate a random number using 32bit xorshift
		*state ^= *state << 13;
		*state ^= *state >> 17;
		*state ^= *state << 5;

		randomFloat = (*state / (float) UINT32_MAX);

		// Generate pink noise using the Voss-McCartney algorithm, only use 4 samples
		n = 0;
		sum = 0.0f;
		for (n = 0; n < 8; n++) {
			pinkBuffer[n] = (pinkBuffer[n] + randomFloat) * 0.5f;
			sum += pinkBuffer[n];
		}
		pinkNoiseBuffer[i] = sum * 0.125f;

		i++;
	}
	d_print("%.2f ms - pink noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *generateBrownNoise(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *lowFrequencyNoiseBuffer = threadData->buffer0;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Engine *engine = threadData->engine;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	double phase = 0.0f;
	float timeStep = 1.0f / project->sampleRate;

	while (i < scene->sampleCount) {
		// Generate a random number using 32bit xorshift
		*state ^= *state << 13;
		*state ^= *state >> 17;
		*state ^= *state << 5;

		// Generate low frequency noise
		phase += TAU * (*state / (float) UINT32_MAX) * timeStep * engine->lowFrequencyNoiseFrequency;
		lowFrequencyNoiseBuffer[i] = sin(phase);

		i++;
	}

	d_print("%.2f ms - low frequency noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *generateLowFrequencyNoise(void *arg) {
	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *brownNoiseBuffer = threadData->buffer0;
	
	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	double lastBrown = 0.0f;

	while (i < scene->sampleCount) {
		*state ^= *state << 13;
		*state ^= *state >> 17;
		*state ^= *state << 5;

		lastBrown += (((*state / (float) UINT32_MAX) * 2.0f) - 1.0f) * 0.02f;

		// Clamp the result
		if (lastBrown > 1.0f) lastBrown = 1.0f;
		if (lastBrown < -1.0f) lastBrown = -1.0f;

		brownNoiseBuffer[i] = lastBrown;

		i++;
	}

	d_print("%.2f ms - brown noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}


// STAGE 2
// Render the base frequencies
void *renderBase(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	// Get data from threadData
	float *baseBuffer = threadData->buffer0;
	double *phaseBuffer = (double *) threadData->buffer1; // Lying a little more
	float *loadBuffer = threadData->buffer2;
	float *frequencyBuffer = threadData->buffer3;
	float *lowFrequencyNoiseMultiplierBuffer = threadData->buffer4;
	float *lowFrequencyNoiseBuffer = threadData->buffer5;
	//float *rpmBuffer = threadData->buffer6;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Engine *engine = threadData->engine;

	uint64_t i = 0;
	uint8_t n = 0;

	int harmonics = engine->harmonics + 1;

	// For multiplication
	float inverseMinimumVolume = 1.0f - engine->minimumVolume;

	// For rpm multiplication
	float rpmVolumeMultiplier = engine->rpmVolumeMultiplier * 60.0f / (2.0f / engine->stroke) / engine->cylinderCount;

	// Used to normalize, power of two so I have the absolute value later
	float absoluteMax = 0.0f;

	// Calculate base frequency and 'n' harmonic frequencies
	while (i < scene->sampleCount) {
		// Add noise to phase
		phaseBuffer[i] += lowFrequencyNoiseMultiplierBuffer[i] * lowFrequencyNoiseBuffer[i];

		n = 0;
		// Add harmonics
		while (n < harmonics) {
			baseBuffer[i] += sin(phaseBuffer[i] * (n + 1)) * (1.0f / (n + 1));
			n++;
		}

		// Get absolute maximum value
		if (absoluteMax < baseBuffer[i] * baseBuffer[i]) {
			absoluteMax = baseBuffer[i] * baseBuffer[i];
		}

		i++;
	}
	i = 0;

	absoluteMax = sqrtf(absoluteMax);

	// Normalize to -1.0f to 1.0f
	if (absoluteMax != 0.0f) {
		while (i < scene->sampleCount) {
			baseBuffer[i] /= absoluteMax;
			i++;
		}
	}
	i = 0;

	// Apply volume multiplication
	while (i < scene->sampleCount) {
		// Multiply with load
		baseBuffer[i] *= (loadBuffer[i] * engine->loadVolumeMultiplier * inverseMinimumVolume) + engine->minimumVolume;

		// Multiply with rpm
		baseBuffer[i] *= ((frequencyBuffer[i] / engine->maxRpm) * rpmVolumeMultiplier * inverseMinimumVolume) + engine->minimumVolume;


		// Check values
		if (baseBuffer[i] > 1.0f && baseBuffer[i] < -1.0f) {
			e_fatal("base amplitude exceeded maximum value at time [%.2f s]\n", i / (float) project->sampleRate);
			threadData->failed = true;
			return NULL;
		}

		i++;
	}

	d_print("%.2f ms - base renedering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *renderValvetrain(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *valvetrainBuffer = threadData->buffer0;
	double *phaseBuffer = (double *) threadData->buffer1;

	struct Scene *scene = threadData->scene;

	uint64_t i = 0;

	while (i < scene->sampleCount) {
		valvetrainBuffer[i] = sin(phaseBuffer[i]);
		i++;
	}

	d_print("%.2f ms - valvetrain rendering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *renderMechanical(void *arg) {
 	time_t startTime = clock();

	//struct ThreadData *threadData = (struct ThreadData *) arg;

	if (false) printf("%p\n", arg);
	d_print("%.2f ms - mechanical rendering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;
}

void *renderVibration(void *arg) {
 	time_t startTime = clock();

	//struct ThreadData *threadData = (struct ThreadData *) arg;

	if (false) printf("%p\n", arg);
	d_print("%.2f ms - vibration rendering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;
}

// STAGE 3
void *joinBuffers(void *arg) {
 	time_t startTime = clock();

	//struct ThreadData *threadData = (struct ThreadData *) arg;

	if (false) printf("%p\n", arg);
	d_print("%.2f ms - join finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;

}
