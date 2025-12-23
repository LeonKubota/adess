#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <math.h>
#include <stdio.h>

#include "utils.h"
#include "main.h"
#include "render/render-general.h"

// Make some constants (from my handy 'CASIO fx-85CE X CLASSWIZ')
const float PI = 3.141592654f;
const float TAU = 2 * PI;

// I have to interpolate keys to rpm and calculate the sine in one step to avoid loss of data (uint8_t can't fit the rpm range)
void keysToSine(void *voidBuffer, int8_t type, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate) {
	// Put the array in the correct type
	switch (type) {
		case 8: {
			uint8_t *buffer = (uint8_t *) voidBuffer;

			keysToSine8(buffer, keyframes, keyframeCount, sampleCount, sampleRate);
			break;
		}
		case 16: {
			int16_t *buffer = (int16_t *) voidBuffer;

			keysToSine16(buffer, keyframes, keyframeCount, sampleCount, sampleRate);
			break;
		}
		case 24: { // I know it uses 32 bits, but it's supposed to be that way
			uint8_t *buffer = (uint8_t *) voidBuffer;

			keysToSine24(buffer, keyframes, keyframeCount, sampleCount, sampleRate);
			break;
		}
		case 32: {
			float *buffer = (float *) voidBuffer;

			keysToSine32(buffer, keyframes, keyframeCount, sampleCount, sampleRate);
			break;
		}
		default: // This should NEVER happen
			e_fatal("The developer of this program, Leon Kubota, is an idiot and f*cked up big time\n");
			return;
	}
}

// Here I am practicing the RY principle: Repeat Yourself
// Because repetition is the mother of wisdom (opakování je matka moudrosti)

// These functions do the processing
void keysToSine8(uint8_t *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate) {
	uint64_t i = 0;

	// Because uint8_t is not big enough for these:
	float currRpm = 0.0f;
	float frequency = 0.0f;
	double phase = 0.0f;

	float timeStep = 1 / (float) sampleRate;

	// If there is only one keyframe
	if (keyframeCount == 1) {
		currRpm = (float) keyframes[0].rpm;
		frequency = currRpm; // TODO calculate real frequency based on rpm
		while (i < sampleCount) {
			phase += TAU * frequency * timeStep; // The iterative approach is surprisingly more stable than '(TAU * frequency * i) / sampleRate'
			buffer[i] = (sin(phase) + 1.0f) * 128;
			i++;
		}
	} else {
		uint16_t prevKey, nextKey = 0;
		float prevTime, nextTime = 0.0f;
		uint16_t prevRpm, nextRpm = 0;
	
		float currentTime = 0.0f;
		float endTime = (float) sampleCount / (float) sampleRate;

		while (i < sampleCount) {
			currentTime = (float) i / (float) sampleRate;
	
			while (nextKey < keyframeCount) {
				if (keyframes[nextKey].keytime > currentTime) {
					break;
				}
				nextKey++;
			}
	
			prevKey = nextKey - 1;
			
			// If the next key is the first key (I am a GENIUS)
			if (prevKey == (uint16_t) -1) { // Cast -1 to unsigned integer to get maximum = there is no 'prevKey'!!!
				prevTime = 0;
				prevRpm = keyframes[nextKey].rpm;
			} else {
				prevTime = keyframes[prevKey].keytime;
				prevRpm = keyframes[prevKey].rpm;
			}
	
			if (nextKey >= keyframeCount) {
				nextTime = endTime;
				nextRpm = keyframes[prevKey].rpm;
			} else {
				nextTime = keyframes[nextKey].keytime;
				nextRpm = keyframes[nextKey].rpm;
			}
	
			// This is the rpm
			currRpm = prevRpm + (currentTime - prevTime) * ((nextRpm - prevRpm) / (nextTime - prevTime));

			// Convert rpm to frequency
			frequency = currRpm;
			// TODO ^ make this a function like this: float rpmToFreq(float currRpm);

			phase += TAU * frequency * timeStep;

			buffer[i] = (sin(phase) + 1.0f) * 128;

			i++;
		}
	}
}

void keysToSine16(int16_t *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate) {
	uint64_t i = 0;

	float currRpm = 0.0f;
	float frequency = 0.0f;
	double phase = 0.0f;

	float timeStep = 1 / (float) sampleRate;
	// Fill the buffer with rpm data from keyframes
	if (keyframeCount == 1) {
		currRpm = (float) keyframes[0].rpm;
		frequency = currRpm; // TODO calculate real frequency based on rpm
		while (i < sampleCount) {
			phase += TAU * frequency * timeStep;
			buffer[i] = sin(phase) * 32767;
			i++;
		}
	} else {
		uint16_t prevKey, nextKey = 0;
		float prevTime, nextTime = 0.0f;
		uint16_t prevRpm, nextRpm = 0;
	
		float currentTime = 0.0f;
		float endTime = (float) sampleCount / (float) sampleRate;
	
		while (i < sampleCount) {
			currentTime = (float) i / (float) sampleRate;
	
			// Calculate the next chronological keyframe
			while (nextKey < keyframeCount) {
				if (keyframes[nextKey].keytime > currentTime) {
					break;
				}
				nextKey++;
			}
			
			prevKey = nextKey - 1;
	
			// If the next key is the first key (I am a GENIUS) 
			if (prevKey == (uint16_t) -1) { // Cast -1 to unsigned integer to get maximum = there is no 'prevKey'!!!
				prevTime = 0;
				prevRpm = keyframes[nextKey].rpm;
			} else {
				prevTime = keyframes[prevKey].keytime;
				prevRpm = keyframes[prevKey].rpm;
			}
	
			if (nextKey >= keyframeCount) {
				nextTime = endTime; 
				nextRpm = keyframes[prevKey].rpm;
			} else {
				nextTime = keyframes[nextKey].keytime;
				nextRpm = keyframes[nextKey].rpm;
			}
			
			currRpm = prevRpm + (currentTime - prevTime) * ((nextRpm - prevRpm) / (nextTime - prevTime));

			frequency = currRpm; // TODO same here
			phase += TAU * frequency * timeStep;
			buffer[i] = sin(phase) * 32767;

			i++;
		}
	}
}

// 24 bit is strange (3 X uint8_t per sample)
void keysToSine24(uint8_t *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate) {
	uint64_t i = 0;

	// Because uint8_t is not big enough for these:
	float currRpm = 0.0f;
	float frequency = 0.0f;
	double phase = 0.0f;

	int32_t combinedSample = 0;

	float timeStep = 1 / (float) sampleRate;
	// If there is only one keyframe
	if (keyframeCount == 1) {
		currRpm = (float) keyframes[0].rpm;
		frequency = currRpm; // TODO calculate real frequency based on rpm

		while (i < sampleCount * 3) {
			combinedSample = (int32_t) (sin(phase) * 8388607);
			phase += TAU * frequency * timeStep;

			// Split and write data in 3 parts
			buffer[i] = (uint8_t) (combinedSample & 0xFF);
			buffer[i + 1] = (uint8_t) ((combinedSample >> 8) & 0xFF);
			buffer[i + 2] = (uint8_t) ((combinedSample >> 16) & 0xFF);

			// Skip over sample (3)
			i+=3;
		}
	} else {
		uint16_t prevKey, nextKey = 0;
		float prevTime, nextTime = 0.0f;
		uint16_t prevRpm, nextRpm = 0;

		float currentTime = 0.0f;
		float endTime = (float) sampleCount / (float) sampleRate;

		float timeStep = 1 / (float) sampleRate;

		while (i < sampleCount * 3) {
			currentTime = (float) i / 3.0f / (float) sampleRate; // Don't forget to divide by 3
			
			while (nextKey < keyframeCount) {
				if (keyframes[nextKey].keytime > currentTime) {
					break;
				}
				nextKey++;
			}
			
			prevKey = nextKey - 1;
		
			// If the next key is the first key (I am a GENIUS)
			if (prevKey == (uint16_t) -1) { // Cast -1 to unsigned integer to get maximum = there is no 'prevKey'!!!
				prevTime = 0;
				prevRpm = keyframes[nextKey].rpm;
			} else {
				prevTime = keyframes[prevKey].keytime;
				prevRpm = keyframes[prevKey].rpm;
			}
			
			if (nextKey >= keyframeCount) {
				nextTime = endTime;
				nextRpm = keyframes[prevKey].rpm;
			} else {
				nextTime = keyframes[nextKey].keytime;
				nextRpm = keyframes[nextKey].rpm;
			}


			// This is the rpm
			currRpm = prevRpm + (currentTime - prevTime) * ((nextRpm - prevRpm) / (nextTime - prevTime));

			frequency = currRpm; // TODO
		
			phase += TAU * frequency * timeStep;

			combinedSample = (int32_t) (sin(phase) * 8388607);

			// Split and write data in 3 parts
			buffer[i] = (uint8_t) (combinedSample & 0xFF);
			buffer[i + 1] = (uint8_t) ((combinedSample >> 8) & 0xFF);
			buffer[i + 2] = (uint8_t) ((combinedSample >> 16) & 0xFF);

			i+=3;
		}
	}
}

void keysToSine32(float *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate) {
	uint64_t i = 0;

	double phase = 0.0f;
	float timeStep = 1 / (float) sampleRate;

	if (keyframeCount == 1) {
		while (i < sampleCount) {
			buffer[i] = keyframes[0].rpm;
			phase += TAU * buffer[i] * timeStep;
			buffer[i] = sin(phase);
			i++;
		}
		return;
	} else {
		uint16_t prevKey, nextKey = 0;
		float prevTime, nextTime = 0.0f;
		uint16_t prevRpm, nextRpm = 0;
		
		float currentTime = 0.0f;
		float endTime = (float) sampleCount / (float) sampleRate;
		
		while (i < sampleCount) {
			currentTime = (float) i / (float) sampleRate;

			while (nextKey < keyframeCount) {
				if (keyframes[nextKey].keytime > currentTime) {
					break;
				}
				nextKey++;
			}
			
			prevKey = nextKey - 1;

			// If the next key is the first key (I am a GENIUS)
			if (prevKey == (uint16_t) -1) { // Cast -1 to unsigned integer to get maximum = there is no 'prevKey'!!!
				prevTime = 0;
				prevRpm = keyframes[nextKey].rpm;
			} else {
				prevTime = keyframes[prevKey].keytime;
				prevRpm = keyframes[prevKey].rpm;
			}

			if (nextKey >= keyframeCount) {
				nextTime = endTime;
				nextRpm = keyframes[prevKey].rpm;
			} else {
				nextTime = keyframes[nextKey].keytime;
				nextRpm = keyframes[nextKey].rpm;
			}

			// Set the rpm
			buffer[i] = prevRpm + (currentTime - prevTime) * ((nextRpm - prevRpm) / (nextTime - prevTime));
			
			i++;
		}
	}

	i = 0;	

	// Splitting = approx. 12% speed increase
	while (i < sampleCount) {
		phase += TAU * buffer[i] * timeStep;
		buffer[i] = sin(phase);
		i++;
	}
}

// Print the buffer
void printNthBuffer(void *voidBuffer, int8_t type, int n, uint64_t sampleCount) {
	uint64_t i = 0;

	switch (type) {
		case 8: {
			uint8_t *buffer = (uint8_t *) voidBuffer;

			while (i < sampleCount) {
				printf("%i\t", buffer[i]);
				i += n;
				if (i % (n*12) == 0) {
					printf("\n");
				}
			}
			break;
		}
		case 16: {
			int16_t *buffer = (int16_t *) voidBuffer;

			while (i < sampleCount) {
				printf("%i\t", buffer[i]);
				i += n;
				if (i % (n*12) == 0) {
					printf("\n");
				}
			}
			break;
		}
		case 24: {
			int32_t *buffer = (int32_t *) voidBuffer;
	
			while (i < sampleCount) {
				printf("%i\t", buffer[i]);
				i += n;
				if (i % (n*12) == 0) {
					printf("\n");
				}
			}
			break;
		}
		case 32: {
			float *buffer = (float *) voidBuffer;

			while (i < sampleCount) {
				printf("%.1f\t", buffer[i]);
				i += n;
				if (i % (n*12) == 0) {
					printf("\n");
				}
			}
			break;
		}
	}
}
