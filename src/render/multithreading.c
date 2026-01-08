#include <stdint.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <pthread.h>

#include "main.h"
#include "utils.h"
#include "commands/render.h"
#include "render/multithreading.h"
#include "render/pitch-shift.h"

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

void *generateLowFrequencyNoise(void *arg) {
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

void *generateStableBrownNoise(void *arg) {
	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	float *stableBrownNoiseBuffer = threadData->buffer0;
	
	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;

	uint32_t *state = (uint32_t *) &project->seed;

	uint64_t i = 0;

	double lastBrown = 0.0f;

	while (i < scene->sampleCount) {
		*state ^= *state << 13;
		*state ^= *state >> 17;
		*state ^= *state << 5;

		lastBrown += ((*state / (double) UINT32_MAX) * 2.0f - 1.0f) * 0.02f;

		// Clamp the result
		if (lastBrown > 1.0f) lastBrown = 1.0f;
		if (lastBrown < -1.0f) lastBrown = -1.0f;

		stableBrownNoiseBuffer[i] = lastBrown;

		i++;
	}
	i = 0;

	int n = 0;
	while (n < 8) {
		while (i < scene->sampleCount) {
			if (i > 1 && i < scene->sampleCount - 1) {
				stableBrownNoiseBuffer[i] = (0.25f * stableBrownNoiseBuffer[i - 1]) + (0.5f * stableBrownNoiseBuffer[i]) + (0.25f * stableBrownNoiseBuffer[i + 1]);
			}
			i++;
		}
		i = 0;
		n++;
	}

	d_print("%.2f ms - stable brown noise generation finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}


// STAGE 2
// Render the base frequencies
void *renderBase(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	// Get data from threadData
	float *baseBuffer = threadData->buffer0;

	// Initialize 'baseBuffer'

	double *phaseBuffer = (double *) threadData->buffer1; // Lying a little more
	float *loadBuffer = threadData->buffer2;
	float *frequencyBuffer = threadData->buffer3;
	float *lowFrequencyNoiseMultiplierBuffer = threadData->buffer4;
	float *lowFrequencyNoiseBuffer = threadData->buffer5;
	float *stableBrownNoiseBuffer = threadData->buffer6;

	//struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Engine *engine = threadData->engine;

	uint64_t i = 0;
	uint8_t n = 0;

	int harmonics = engine->harmonics + 1;

	// For multiplication
	float inverseMinimumVolume = 1.0f - engine->minimumVolume;
	float inverseMinimumNoise = 1.0f - engine->minimumNoise;

	// For rpm multiplication
	float rpmVolumeMultiplier = engine->rpmVolumeMultiplier * 60.0f / (2.0f / engine->stroke) / engine->cylinderCount;

	// Used to normalize
	float absoluteMax = 0.0f;

	// Calculate base frequency and 'n' harmonic frequencies
	while (i < scene->sampleCount) {
		// Add noise to phase of low frequencies
		phaseBuffer[i] += lowFrequencyNoiseMultiplierBuffer[i] * lowFrequencyNoiseBuffer[i];

		// Add noise based on load
		phaseBuffer[i] += engine->loadNoiseMultiplier * ((loadBuffer[i] * inverseMinimumNoise) + engine->minimumNoise) * stableBrownNoiseBuffer[i];

		n = 0;
		// Add harmonics
		while (n < harmonics) {
			// This sounds stupid (like wind)
			//baseBuffer[i] += sin(phaseBuffer[i] * (n + 1 + ((n + 1) * (stableBrownNoiseBuffer[i] * 0.001f * loadBuffer[i])))) * (1.0f / (n + 1) * (1 + (stableBrownNoiseBuffer[i] * 0.01f)));
			
			// This doesn't have noise (sounds like a sine wave, because it is one)
// 			baseBuffer[i] += sin(phaseBuffer[i] * (n + 1)) * (1.0f / (n + 1));
			
			// This sounds GREAT! Just like a real engine
			baseBuffer[i] += sin(phaseBuffer[i] * (n + 1)) * (1.0f / (n + 1));
			n++;
		}

		// Get absolute maximum value
		if (absoluteMax < fabs(baseBuffer[i])) {
			absoluteMax = fabs(baseBuffer[i]);
		}

		i++;
	}
	i = 0;

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
		// Add a tiny bit noise
		baseBuffer[i] *= (stableBrownNoiseBuffer[i] * inverseMinimumNoise * 0.1f) + ((1.0f / 0.1f) * engine->minimumNoise);

		// Multiply with load
		baseBuffer[i] *= (loadBuffer[i] * engine->loadVolumeMultiplier * inverseMinimumVolume) + engine->minimumVolume;

		// Multiply with rpm
		baseBuffer[i] *= ((frequencyBuffer[i] / engine->maxRpm) * rpmVolumeMultiplier * inverseMinimumVolume) + engine->minimumVolume;

		// Get absolute maximum value
		if (absoluteMax < fabs(baseBuffer[i])) {
			absoluteMax = fabs(baseBuffer[i]);
		}

		i++;
	}
	i = 0;

	// Normalize to -1.0f to 1.0f
	if (absoluteMax != 0.0f) {
		while (i < scene->sampleCount) {
			baseBuffer[i] /= absoluteMax;
			i++;
		}
	}

	d_print("%.2f ms - base renedering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *renderValvetrain(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Engine *engine = threadData->engine;

	// Don't calculate valvetrain for 2 stroke engines
	if (engine->stroke == 2) {
		d_print("%.2f ms - valvetrain rendering finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);
		return NULL;
	}

	float *valvetrainBuffer = threadData->buffer0;
	double *phaseBuffer = (double *) threadData->buffer1;
	float *rpmBuffer = threadData->buffer2;
	//float *frequencyBuffer = threadData->buffer3;
	float *pinkNoiseBuffer = threadData->buffer4;

	double phase = 0.0f;
	float timeStep = 1.0f / project->sampleRate;
	float frequency = 0.0f;
	float intakeMultiplier = 0;
	float exhaustMultiplier = 0;

	float offset = engine->valvetrainTimingOffset;

	uint64_t i = 0;

	while (i < scene->sampleCount) {
		// Intake valve sound
		// Formula: ((rps / 2) * cylinderCount) / 4 / 2
		// Divide by four so we only get the intake and divide by 2 because I use abs later
		frequency = (rpmBuffer[i] / 120.0f) * engine->cylinderCount * 0.125f;
		phase += TAU * frequency * timeStep;

		// So that I don't read out of bounds
		if (i < scene->sampleCount - 1) {
			intakeMultiplier = (fabs(sin(phase + 0.1f * pinkNoiseBuffer[i + 1])) - 0.9f) * 10.0f;
		} else {
			intakeMultiplier = (fabs(sin(phase + 0.1f * pinkNoiseBuffer[i])) - 0.9f) * 10.0f;
		}

		if (intakeMultiplier < 0.0f) intakeMultiplier = 0.0f;

		exhaustMultiplier = (fabs(sin(phase + offset + 0.1f * pinkNoiseBuffer[i])) - 0.9f) * 10.0f;
		if (exhaustMultiplier < 0.0f) exhaustMultiplier = 0.0f;

		// Saw wave generation
		valvetrainBuffer[i] = 2.0f * (fmod(phaseBuffer[i] * 2.0f, TAU) / TAU) - 1.0f;

		// Multiply (make it 'click')
		valvetrainBuffer[i] *= 0.5f * (pow(intakeMultiplier, 2) + pow(exhaustMultiplier, 2));

		if (valvetrainBuffer[i] > 1.0f) printf("err\n");
		if (valvetrainBuffer[i] < -1.0f) printf("err\n");
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
void *combineBuffers(void *arg) {
 	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	//struct Project *project = threadData->project;
	struct Scene *scene = threadData->scene;
	struct Engine *engine = threadData->engine;

	float *combinedBuffer = threadData->buffer0;
	float *baseBuffer = threadData->buffer1;
	float *valvetrainBuffer = threadData->buffer2;
	//float *mechanicalBuffer = threadData->buffer3;

	uint64_t i = 0;

	bool overflowed = false;

	while (i < scene->sampleCount) {
		combinedBuffer[i] += baseBuffer[i] * engine->baseVolume;
		combinedBuffer[i] += valvetrainBuffer[i] * engine->valvetrainVolume;
		//combinedBuffer[i] += mechanicalBuffer[i] * engine->mechanicalVolume;
		//
		if (fabs(combinedBuffer[i]) > 1.0f) overflowed = true;

		i++;
	}

	if (overflowed) {
		e_warning("maximum amplitude exceeded while combining buffers\n");
	}

	d_print("%.2f ms - join finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}


// STAGE 4
void *postProcess(void *arg) {
	time_t startTime = clock();

	struct ThreadData *threadData = (struct ThreadData *) arg;

	struct Scene *scene = threadData->scene;

	uint64_t i = 0;
	uint64_t n = 0;

	float *postProcessedBuffer = threadData->buffer0;
	float *combinedBuffer = threadData->buffer1;
	float *stableBrownNoiseBuffer = threadData->buffer2;


	// Set up threads
	int pitchCount = 3;

	pthread_t pitchThreads[pitchCount];

	struct PitchShiftData {
		float *buffer;
		float *noiseBuffer;
		struct Scene *scene;
		uint8_t factor;
	} pitchShiftDataArray[pitchCount];

	// Set up buffers for pitch shifts
	while (i < (uint64_t) pitchCount) {
		// Put things into data struct
		pitchShiftDataArray[i].buffer = (float *) malloc(scene->sampleCount * sizeof(float));
		pitchShiftDataArray[i].noiseBuffer = stableBrownNoiseBuffer;
		pitchShiftDataArray[i].scene = scene;
		pitchShiftDataArray[i].factor = (2 * i + 2);	

		// Handle error
		if (pitchShiftDataArray[i].buffer == NULL) {
			threadData->failed = true;
			return NULL;
		}

		// Copy data into 'pitchShiftDataArray[i].buffer', add noise
		while (n < scene->sampleCount) {
			pitchShiftDataArray[i].buffer[n] = combinedBuffer[n] + (stableBrownNoiseBuffer[n] * i);
			n++;
		}
		n = 0;

		if (pthread_create(&pitchThreads[i], NULL, pitchShiftThread, (void *) &pitchShiftDataArray[i]) != 0) {
			threadData->failed = true;
			return NULL;
		}
		i++;
	}
	i = 0;

	// Join the threads
	while (i < (uint64_t) pitchCount) {
		if (pthread_join(pitchThreads[i], NULL) != 0) {
			threadData->failed = true;
			return NULL;
		}
		i++;
	}
	i = 0;

	float absoluteMaximum = 0.0f;

	float pitchAmplitude[pitchCount];
	pitchAmplitude[0] = 0.0f;
	pitchAmplitude[1] = 0.0f;
	pitchAmplitude[2] = 0.0f;
	pitchAmplitude[3] = 0.0f;

	// Combine pitch shifted audios
	while (i < scene->sampleCount) {
		// Add the main base
		postProcessedBuffer[i] = combinedBuffer[i];

		// Add harmonic frequencies (to fill up the top)
		while (n < (uint64_t) pitchCount) {
			postProcessedBuffer[i] += pitchShiftDataArray[n].buffer[i] * pitchAmplitude[n];
			n++;
		}
		n = 0;

		// For normalization later
		if (postProcessedBuffer[i] > absoluteMaximum) absoluteMaximum = postProcessedBuffer[i];

		i++;
	}
	i = 0;


	// Free 'pitchShiftDataArray[i].buffer'
	while (i < (uint64_t) pitchCount) {
		free(pitchShiftDataArray[i].buffer);
		i++;
	}
	i = 0;


	// Normalize
	while (i < scene->sampleCount) {
		postProcessedBuffer[i] /= absoluteMaximum;
		i++;
	}

	d_print("%.2f ms - post processing finished\n", (clock() - startTime) * 1000.0f / CLOCKS_PER_SEC);

	return NULL;
}

void *pitchShiftThread(void *arg) {
	struct PitchShiftData {
		float *buffer;
		float *noiseBuffer;
		struct Scene *scene;
		uint8_t factor;
	} *pitchShiftData = (struct PitchShiftData *) arg;

	float *buffer = pitchShiftData->buffer;
	float *noiseBuffer = pitchShiftData->noiseBuffer;
	struct Scene *scene = pitchShiftData->scene;
	uint8_t factor = pitchShiftData->factor;

	pitchShift(buffer, noiseBuffer, factor, scene);

	return NULL;
}
