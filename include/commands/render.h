#include "files/extract.h"
#include "files/check.h"
#include "main.h"

#ifndef RENDER_H
#define RENDER_H

int render(char **args);

int renderAll(char *projectFilePath);
int renderScene(char *rawscenename, char *projectFilePath);

void interpolateKeys(int16_t *bufferkey, int buffersizekey, float *time, float *rpm, float *load);

#endif
