#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "render/fourier.h"
#include "render/modify-amplitudes.h"
#include "render/pitch-shift.h"

#define WINDOW_SIZE 4096
#define HOP_SIZE 1300

void modifyAmplitudes(float *buffer, float cutoff, struct Project *project, struct Scene *scene) {
	float dt = 1.0f / project->sampleRate;
	float RC = 1.0f / (TAU * cutoff);
	float alpha = dt / (RC + dt);

	float *output = (float *) malloc(scene->sampleCount * sizeof(float));

	output[0] = buffer[0];
	uint64_t i = 0;

	while (i < scene->sampleCount) {
		output[i] = output[i - 1] + alpha * (buffer[i] - output[i - 1]);
		i++;
	} i = 0;

	while (i < scene->sampleCount) {
		buffer[i] = output[i];
		i++;
	}

	free(output);
}
