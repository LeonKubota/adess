#include <time.h>

#include "main.h"
#include "commands/render.h"

#ifndef MULTITHREADING_H
#define MULTITHREADING_H

// STAGE 1
void *interpolate(void *arg);
void *generatePinkNoise(void *arg);
void *generateBrownNoise(void *arg);
void *generateLowFrequencyNoise(void *arg);

// STAGE 2
void *renderBase(void *arg);
void *renderValvetrain(void *arg);
void *renderMechanical(void *arg);
void *renderVibration(void *arg);

// STAGE 3
void *joinBuffers(void *arg);

#endif
