#include <complex.h>

#ifndef PITCH_SHIFT_H
#define PITCH_SHIFT_H

#include "main.h"

int pitchShift(float *input, float *noiseBuffer, uint8_t factor, struct Scene *scene);

float *precalculateHanning(uint16_t windowSize);
uint16_t pickWindowSize(uint8_t factor);
uint16_t pickHopSize(uint8_t factor);

#endif
