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
	struct ThreadData *threadData = (struct ThreadData *) arg;

	// Get data from threadData
	float *frequencyBuffer = threadData->buffer1;
	float *loadBuffer = threadData->buffer2;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Keyframe *keyframes = threadData->keyframes;

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

			frequencyBuffer[i] = prevFrequency + (currentTime - prevTime) * ((nextFrequency - prevFrequency) / (nextTime - prevTime));
			loadBuffer[i] = prevLoad + (currentTime - prevTime) * ((nextLoad - prevLoad) / (nextTime - prevTime));

			i++;
		}
	}
	d_print("%.2f ms - interpolation finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

// Generate noise: pink, brown and "low frequency"
void *generateNoise(void *arg) {
	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *pinkNoiseBuffer = threadData->buffer1;
	float *brownNoiseBuffer = threadData->buffer2;
	float *lowFrequencyNoiseBuffer = threadData->buffer3;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	float randomNumber = 0.0f;

	// Used for pink noise
	uint8_t n = 0;
	float pinkSum = 0.0f;
	float pinkBuffer[7] = {0};

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

		randomNumber = *state / (float) UINT32_MAX;

		// Generate pink noise using the Voss-McCartney algorithm
		n = 0;
		pinkSum = 0.0f;
		for (n = 0; n < 7; n++) {
			pinkBuffer[n] = (pinkBuffer[n] + randomNumber) * 0.5f;
			pinkSum += pinkBuffer[n];
		}
		pinkNoiseBuffer[i] = pinkSum * 0.125f;

		// Generate brown noise
		lastBrown += ((randomNumber * 2.0f) - 1.0f) * 0.02f;

		// Clamp it
		if (lastBrown > 1.0f) lastBrown = 1.0f;
		if (lastBrown < -1.0f) lastBrown = -1.0f;
		brownNoiseBuffer[i] = lastBrown;

		// Generate low frequency noise
		phase += TAU * (*state / (float) UINT32_MAX) * timeStep;
		lowFrequencyNoiseBuffer[i] = sin(phase);

		i++;
	}
	d_print("%.2f ms - noise generation finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}


// STAGE 2
// Render the base frequencies
void *renderBase(void *arg) {
	if (false) printf("%p\n", arg);
	d_print("%.2f ms - base renedering finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;
}

void *renderValvetrain(void *arg) {
	if (false) printf("%p\n", arg);
	d_print("%.2f ms - valvetrain rendering finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;
}

void *renderMechanical(void *arg) {
	if (false) printf("%p\n", arg);
	d_print("%.2f ms - mechanical rendering finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;
}

void *renderVibration(void *arg) {
	if (false) printf("%p\n", arg);
	d_print("%.2f ms - vibration rendering finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;
}

// STAGE 3
void *joinBuffers(void *arg) {
	if (false) printf("%p\n", arg);
	d_print("%.2f ms - join finished\n", (clock() - g_startTime) * 1000.0f / CLOCKS_PER_SEC);
	return NULL;

}
