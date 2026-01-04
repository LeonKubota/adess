#include <stdint.h>

#include "main.h"

#ifndef RENDER_GENERAL_H
#define RENDER_GENERAL_H 

// Type: 8 - 8bit; 16 - 16bit; 24 - 24bit; 32 - 32bit;
void keysToSine(void *voidBuffer, int8_t type, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate, uint32_t *state);

// Process for different types of buffers
void keysToSine8(uint8_t *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate, uint32_t *state);
void keysToSine16(int16_t *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate, uint32_t *state);
void keysToSine24(uint8_t *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate);
void keysToSine32(float *buffer, struct Keyframe *keyframes, int keyframeCount, uint64_t sampleCount, int sampleRate);

// Random generation of numebers
float generateNext(float previous, uint32_t *state);

// Type is the same
void printNthBuffer(void *voidBuffer, int8_t type, int n, uint64_t sampleCount);

#endif
