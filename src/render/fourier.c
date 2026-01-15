// Both of these functions are modified implementations (real numbers only) of the fast Fourier Transform (Cooley-Tukey algorithm)

// The original implementation
// https://www.math.wustl.edu/~victor/mfmm/fourier/fft.c

// I don't really understand it :(

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <complex.h> // For complex number arithmetics

#include "main.h"


// Math magic
void fastFourierTransform(complex float *input, uint64_t n, complex float *temp) {
	if (n > 1) {
		uint64_t k, m;
		complex float w = 0.0f + (0.0f * I);
		complex float z = 0.0f + (0.0f * I);
		complex float *even = temp; 
		complex float *odd = temp + (n / 2);

		for (k = 0; k < (n / 2); k++) {
			even[k] = input[2 * k];
			odd[k] = input[2 * k + 1];
		}

		// Recursion magic
		fastFourierTransform(even, n / 2, input);
		fastFourierTransform(odd, n / 2, input);

		for (m = 0; m < (n / 2); m++) {
			w = cos(TAU * m / (float) n) - (sin(TAU * m / (float) n) * I);
			z = w * odd[m];
			input[m] = even[m] + z;
			input[m + (n / 2)] = even[m] - z;
		}
	}
}

// Inverse math magic
void inverseFastFourierTransform(complex float *input, uint64_t n, complex float *temp) {
	if (n > 1) {
		uint64_t k, m;
		complex float w, z;
		complex float *even = temp;
		complex float *odd = temp + (n / 2);

		for (k = 0; k < (n / 2); k++) {
			even[k] = input[2 * k];
			odd[k] = input[2 * k + 1];
		}

		// Recursion magic
		inverseFastFourierTransform(even, n / 2, input);
		inverseFastFourierTransform(odd, n / 2, input);

		for (m = 0; m < (n / 2); m++) {
			w = cos(TAU * m / (float) n) + (sin(TAU * m / (float) n) * I);
			z = w * odd[m];
			input[m] = (even[m] + z) / 2.0f;
			input[m + (n / 2)] = (even[m] - z) / 2.0f;
		}
	}
}
