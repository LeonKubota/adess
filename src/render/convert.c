#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <math.h>
#include <stdio.h>

#include "utils.h"
#include "main.h"
#include "render/convert.h"

void convert(void *voidBuffer, float *floatBuffer, uint64_t sampleCount, uint8_t bitDepth) {
	// Perform correct conversion and scaling based on bitDepth
	uint64_t i = 0;

	switch (bitDepth) {
		case 8: {
			uint8_t *buffer = (uint8_t *) voidBuffer;

			while (i < sampleCount) {
				buffer[i] = (floatBuffer[i] + 1.0f) * 128;
				i++;
			}

			break;
		}
		case 16: {
			int16_t *buffer = (int16_t *) voidBuffer;

			while (i < sampleCount) {
				buffer[i] = floatBuffer[i] * 32767;
				i++;
			}

			break;
		}
		case 24: {
			uint8_t *buffer = (uint8_t *) voidBuffer; // Looks the same as 8bit, but it's 3 times longer

			uint64_t k = 0;
			int32_t currentSample = 0;

			while (i < sampleCount) {
				currentSample = (floatBuffer[i] * 8388607);

				buffer[k] = (uint8_t) ((currentSample) & 0xFF);
				buffer[k + 1] = (uint8_t) ((currentSample >> 8) & 0xFF);
				buffer[k + 2] = (uint8_t) ((currentSample >> 16) & 0xFF);

				i++;
				k += 3;
			}

			break;
		}
		case 32: {
			float *buffer = (float *) voidBuffer;

			while (i < sampleCount) {
				buffer[i] = floatBuffer[i];
				i++;
			}

			break;
		}
	}
}
