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

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Keyframe *keyframes = threadData->keyframes;

	double phase = 0.0f;
	float timeStep = 1.0f / project->sampleRate;

	uint64_t i = 0;

	// If there is only one keyframe
	if (scene->keyframeCount == 1) {
		while (i < scene->sampleCount) {
			frequencyBuffer[i] = keyframes[0].rpm;
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

// Generate noise: pink, brown and "low frequency"
void *generateNoise(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *brownNoiseBuffer = threadData->buffer0;
	float *lowFrequencyNoiseBuffer = threadData->buffer1;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	float randomFloat = 0.0f;

	// Used for brown noise
	double lastBrown = 0.0f;

	// Used for low frequency noise
	double phase = 0.0f;
	float timeStep = 1.0f / project->sampleRate;

	while (i < scene->sampleCount) {
		// Generate a random number using 32bit xorshift
		*state ^= *state << 13;
		*state ^= *state >> 17;
		*state ^= *state << 5;

		randomFloat = *state / (float) UINT32_MAX;

		// Generate brown noise
		lastBrown += ((randomFloat * 2.0f) - 1.0f) * 0.02f;

		// Clamp it
		if (lastBrown > 1.0f) lastBrown = 1.0f;
		if (lastBrown < -1.0f) lastBrown = -1.0f;
		brownNoiseBuffer[i] = lastBrown;

		// Generate low frequency noise
		phase += TAU * randomFloat * timeStep;
		lowFrequencyNoiseBuffer[i] = sin(phase);

		i++;
	}
	d_print("%.2f ms - brown and low frequency noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *generatePinkNoise(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *pinkNoiseBuffer = threadData->buffer0;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	uint8_t n = 0;
	float sum = 0.0f;
	float pinkBuffer[4] = {0};

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
		for (n = 0; n < 4; n++) {
			pinkBuffer[n] = (pinkBuffer[n] + randomFloat) * 0.5f;
			sum += pinkBuffer[n];
		}
		pinkNoiseBuffer[i] = sum * 0.25f;

		i++;
	}
	d_print("%.2f ms - pink noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

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
	//float *loadBuffer = threadData->buffer2;
	//float *pinkNoiseBuffer = threadData->buffer3;
	//float *brownNoiseBuffer = threadData->buffer4;
	//float *lowFrequencyNoiseBuffer = threadData->buffer5;

	struct Scene *scene = threadData->scene;

	uint64_t i = 0;

	while (i < scene->sampleCount) {
		//printf("phase: %f\n", phaseBuffer[i]);
		baseBuffer[i] = sin(phaseBuffer[i]);
		i++;
	}
	d_print("%.2f ms - base renedering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *renderValvetrain(void *arg) {
 	time_t startTime = clock();

	//struct ThreadData *threadData = (struct ThreadData *) arg;

	if (false) printf("%p\n", arg);
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
