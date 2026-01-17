#include "files/extract.h"
#include "files/check.h"
#include "main.h"
#include <stdint.h>

#ifndef EXPORT_H
#define EXPORT_H

int makeWavHeader(FILE *file, uint32_t sampleRate, uint16_t bitDepth, uint64_t sampleCount);

#endif
