#include <stdbool.h>
#include <stdint.h>

#ifndef EXTRACT_H
#define EXTRACT_H

#include "extract.h"

char *parseLineValueS(char *variable, char *path);
int64_t parseLineValueI(char *variable, char *path);
float parseLineValueF(char *variable, char *path);
bool parseLineValueB(char *variable, char *path);

char *parseLineValue(char *variable, char *path);
bool compare(char *char1, char *char2);
int getVariableLineNumber(char *variable, char *path);

void loadKeyframes(char *scenePath, struct Keyframe *keyframes, int keyCount);
int loadEngine(char *enginePath, struct Engine *engine);

#endif
