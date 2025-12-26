#include "files/extract.h"
#include "files/check.h"
#include "main.h"

#ifndef RENDER_H
#define RENDER_H

int render(char **args);

int renderAll(char *projectFilePath);
int renderScene(char *rawscenename, char *projectFilePath, char *name);

// Helper functions in execution order
char *getScenePath(char *sceneNameInput, char *projectPath);
char *getEnginePath(char *scenePath, char *projectPath);
char *getOutputPath(char *name, char *sceneNameInput, char *projectPath);
int getKeyframes(struct Keyframe *keyframes, int keyframeCount, struct Engine *engine, char *scenePath); 

bool sortKeys(struct Keyframe *keyframes, int keyCount);
void rpmToFrequency(struct Keyframe *keyframes, int keyCount, struct Engine *engine);


char *getThingPath(char *thingPath, char *thingName, char *thingType);

// OtherHelpers
char *processName(char *inputName);

// Printing functions
void printKeys(struct Keyframe *keyframesPrint, int keyCount);

#endif
