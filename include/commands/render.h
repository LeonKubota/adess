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
void interpolateKeys(int16_t *buffer, uint64_t length, struct Keyframe *keyframes);

bool sortKeys(struct Keyframe *keyframes, int keyCount);

// Printing functions
void printKeys(struct Keyframe *keyframesPrint, int keyCount);
void printEveryNBuffer(const int16_t *buffer, int n, uint64_t length);

#endif
