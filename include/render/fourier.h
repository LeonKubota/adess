#include <complex.h>

#ifndef FOURIER_H
#define FOURIER_H

#include "main.h"

void fastFourierTransform(complex double *input, uint64_t n, complex double *temp);
void inverseFastFourierTransform(complex double *input, uint64_t n, complex double *temp);

#endif
