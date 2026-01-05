#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "render/fourier.h"
#include "render/pitch-shift.h"

#define WINDOW_SIZE 1024 // This MUST be 2^n
#define HOP_SIZE 512

int pitchShift(float *input, float *noiseBuffer, uint8_t factor, struct Scene *scene) {
	uint32_t windowCount = 0;

	if (false) printf("%f", noiseBuffer[0]);

	float *outputBuffer = (float *) malloc(factor * scene->sampleCount * sizeof(float));
	if (outputBuffer == NULL) return 1;

	// Calculate window count
	while (windowCount < scene->sampleCount / HOP_SIZE) {
		windowCount++;
	}
	windowCount++; // Add one for safety

	// Make a buffer for one window and for all the windows
	complex float window[WINDOW_SIZE];
	complex float fourierTemp[WINDOW_SIZE];

	float absoluteMaximum = 0.0f;

	uint32_t currentWindow = 0;
	uint64_t n = 0;
	uint32_t offset = 0;

	while (currentWindow < windowCount) {
		offset = currentWindow * HOP_SIZE;

		// Copy data into window and convert to complex numbers
		while (n < WINDOW_SIZE) {
			window[n] = input[offset + n] + (0.0f * I);
			n++;
		}
		n = 0;

		// Fast fourier transform of window (no need for zero padding, it's alread 2^n)
		fastFourierTransform(window, WINDOW_SIZE, fourierTemp);
		
		while (n < WINDOW_SIZE) {
			window[n] *= factor;

			n++;
		}
		n = 0;

		// Go back to samples
		inverseFastFourierTransform(window, WINDOW_SIZE, fourierTemp);

		// Put windows into 'outputBuffer'
		while (n < WINDOW_SIZE) {
			outputBuffer[factor * offset + n] += crealf(window[n]);

			// Used later for normalization
			if (fabs(outputBuffer[factor * offset + n]) > absoluteMaximum) absoluteMaximum = fabs(outputBuffer[factor * offset + n]);

			n++;
		}
		n = 0;

		currentWindow++;
	}

	
	uint64_t i = 0;

	// Normalize the data and copy into the input buffer
	while (i < scene->sampleCount) {
		input[i] = outputBuffer[i * factor] / absoluteMaximum;
		i++;
	}

	free(outputBuffer);

	return 0;
}
