#include <complex.h>

#ifndef PITCH_SHIFT_H
#define PITCH_SHIFT_H

#include "main.h"

int pitchShift(float *input, float *noiseBuffer, uint8_t factor, struct Scene *scene);
float *precalculateHanning(void *);

#endif
