#include "files/extract.h"
#include "files/check.h"
#include "main.h"

#ifndef RENDER_H
#define RENDER_H

int render(char **args);

int renderAll(char *projectFilePath);
int renderScene(char *rawscenename, char *projectFilePath, char *name);

// Helper functions in execution order
char *getThingPath(char *thingPath, char *thingName, char *thingType);
bool sortKeys(struct Keyframe *keyframes, int keyCount);

// Printing functions
void printKeys(struct Keyframe *keyframesPrint, int keyCount);

// Other helpers
char *processName(char *inputName);

#endif
