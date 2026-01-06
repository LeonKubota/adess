#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "render/fourier.h"
#include "render/pitch-shift.h"

#define WINDOW_SIZE 2048 // This MUST be 2^n
#define HOP_SIZE 512

int pitchShift(float *input, float *noiseBuffer, uint8_t factor, struct Scene *scene) {
	uint32_t windowCount = 0;

	float *outputBuffer = (float *) calloc(factor * scene->sampleCount, sizeof(float));
	if (outputBuffer == NULL) return 1;

	// Calculate window count
	while (windowCount < scene->sampleCount / HOP_SIZE) {
		windowCount++;
	}
	windowCount++; // Add one for safety

	// Precalculate Hanning window function
	float *hanning = precalculateHanning(NULL);

	// Make a buffer for one window and for all the windows
	complex float window[WINDOW_SIZE];
	complex float fourierTemp[WINDOW_SIZE] = {0};

	float absoluteMaximum = 0.0f;

	uint32_t currentWindow = 0;
	uint64_t n = 0;
	uint32_t offset = 0;

	while (currentWindow < windowCount) {
		offset = currentWindow * HOP_SIZE + factor;

		// Copy data into window and convert to complex numbers
		while (n < WINDOW_SIZE) {
			if (offset + n < scene->sampleCount) {
				window[n] = input[offset + n] + (0.0f * I);
			} else {
				window[n] = 0 + (0.0f * I);
			}
			n++;
		}
		n = 0;

		// Fast fourier transform of window (no need for zero padding, it's alread 2^n)
		fastFourierTransform(window, WINDOW_SIZE, fourierTemp);
		
		while (n < WINDOW_SIZE) {
			//window[n] *= (factor * (1 + (0.1f * noiseBuffer[n])));
			window[n] *= factor + (0.1f * noiseBuffer[n]);

			n++;
		}
		n = 0;

		// Go back to samples
		inverseFastFourierTransform(window, WINDOW_SIZE, fourierTemp);

		// Multiply by the Hanning window function
		while (n < WINDOW_SIZE) {
			window[n] *= hanning[n];
			n++;
		}
		n = 0;

		// Put windows into 'outputBuffer'
		while (n < WINDOW_SIZE) {
			if (factor * offset + n < factor * scene->sampleCount) {
				outputBuffer[factor * offset + n] += crealf(window[n]);

				// Used later for normalization
				if (fabs(outputBuffer[factor * offset + n]) > absoluteMaximum) absoluteMaximum = fabs(outputBuffer[factor * offset + n]);
			} else {
				break;
			}

			n++;
		}
		n = 0;

		currentWindow++;
	}
	free(hanning);
	
	uint64_t i = 0;

	//float noiseMultiplier = 0.75f * (factor / 38.0f);
	float noiseMultiplier = 0.0f;

	// Normalize the data and copy into the input buffer
	while (i < scene->sampleCount) {
		input[i] = ((outputBuffer[i * factor] / absoluteMaximum) * (1.0f + (noiseMultiplier * noiseBuffer[i])));
		i++;
	}

	free(outputBuffer);

	return 0;
}

float *precalculateHanning(void *shutUpCompiler) {
	if (shutUpCompiler != NULL) printf("what??\n");
	float *hanning = (float *) malloc(WINDOW_SIZE * sizeof(float));

	uint16_t i = 0;

	while (i < WINDOW_SIZE) {
		hanning[i] = 0.5f * (1.0f - cos((TAU * (float) i) / ((float) WINDOW_SIZE - 1.0f)));
		i++;
	}

	return hanning;
}
