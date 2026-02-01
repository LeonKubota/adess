#include "files/extract.h"
#include "main.h"

#ifndef RENDER_H
#define RENDER_H

int render(char **args);

int renderAll(struct Project *project);
int renderScene(char *rawscenename, struct Project *project, char *name);

// Helper functions in execution order
int getScene(struct Scene *scene, char *sceneNameInput, struct Project *project);
int getEngine(struct Scene *scene, struct Engine *engine, struct Project *project);
char *getOutputPath(char *name, char *sceneNameInput, struct Project *project);
int getKeyframes(struct Keyframe *keyframes, struct Scene *scene, struct Engine *engine); 

// Helper functions
bool sortKeys(struct Keyframe *keyframes, int keyCount);
void rpmToFrequency(struct Keyframe *keyframes, int keyCount, struct Engine *engine);

char *getThingPath(char *thingPath, char *thingName, char *thingType);

// OtherHelpers
int getProject(struct Project *project, char *projectFilePath);
char *processName(char *inputName);
uint64_t calculateSmallestPower(uint64_t x);

// Printing functions
/*
void printKeys(struct Keyframe *keyframesPrint, int keyCount);
void printMinMax(float *buffer, struct Scene *scene, struct Project *project);
*/

#endif
