#include <time.h>

#include "main.h"
#include "commands/render.h"

#ifndef MULTITHREADING_H
#define MULTITHREADING_H

// STAGE 1
void *interpolate(void *arg);
void *generatePinkNoise(void *arg);
void *generateStableBrownNoise(void *arg);
void *generateLowFrequencyNoise(void *arg);

// STAGE 2
void *renderBase(void *arg);
void *renderValvetrain(void *arg);
void *renderMechanical(void *arg);
void *renderVibration(void *arg);

// STAGE 3
void *combineBuffers(void *arg);
void *makeComplexBuffer(void *arg);

// STAGE 4
void *postProcess(void *arg);
void *pitchShiftThread(void *arg);

// TEST
void *threadTest(void *arg);

#endif
