#include <stdio.h>
#include <stdint.h>

#include "utils.h"

int makeWavHeader(FILE *file, uint32_t sampleRate, uint16_t bitDepth, uint32_t sampleCount) {
	uint16_t numChannels = 1; // TEMP this could change (it won't)
	uint32_t chunkSize = 36 + (sampleCount * numChannels * (bitDepth / 8));

	// RIFF chunk descriptor
	fwrite("RIFF", 1, 4, file);
	fwrite(&chunkSize, 1, 4, file);
	fwrite("WAVE", 1, 4, file);

	uint32_t subchunk1Size = 16; // 16 for PCM

	uint16_t audioFormat = 1; // 1 for standard PCM
	if (bitDepth == 32) {
		audioFormat = 3; // Use 3 for 32 bit IEEE floating point PCM
	}

	printf("bitDepth: %i\n", bitDepth);
	if (bitDepth == 24) {
	}

	uint32_t byteRate = sampleRate * numChannels * (bitDepth / 8);
	uint16_t blockAlign = numChannels * (bitDepth / 8);

	uint32_t dataSizeBytes = sampleCount * numChannels * (bitDepth / 8);

	// fmt sub-chunk
	fwrite("fmt ", 1, 4, file);
	fwrite(&subchunk1Size, 4, 1, file);
	fwrite(&audioFormat, 2, 1, file);
	fwrite(&numChannels, 2, 1, file);
	fwrite(&sampleRate, 4, 1, file);
	fwrite(&byteRate, 4, 1, file);
	fwrite(&blockAlign, 2, 1, file);
	fwrite(&bitDepth, 2, 1, file);

	// data sub-chunk
	fwrite("data", 1, 4, file);
	fwrite(&dataSizeBytes, 4, 1, file);

	// Don't close the file, there is more data to write!! (the actual sound data)
	// fclose(file);
	return 0;
}
