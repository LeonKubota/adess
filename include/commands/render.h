#include "files/extract.h"
#include "files/check.h"
#include "main.h"

#ifndef RENDER_H
#define RENDER_H

int render(char **args);

int renderAll(char *projectFilePath);
int renderScene(char *rawscenename, char *projectFilePath);

// Helper functions in execution order
char *getThingPath(char *thingPath, char *thingName, char *thingType);
void interpolateKeys(int *bufferkey, int buffersizekey, float *time, int *rpm, float *load);
void printKeys(struct Keyframe *keyframesPrint, int keyCount);

#endif
