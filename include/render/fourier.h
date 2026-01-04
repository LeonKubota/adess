#include <complex.h>

#ifndef FOURIER_H
#define FOURIER_H

#include "main.h"

void fastFourierTransform(complex float *input, uint64_t n, complex float *temp);
void inverseFastFourierTransform(complex float *input, uint64_t n, complex float *temp);

#endif
