#include<stdbool.h>

#ifndef CHECK_H
#define CHECK_H

bool checkFileExistsIn(char *path, const char *filename);
bool checkFileExists(char *path);
char *findProjectFile(char *path);
char *getCurDirectory(char *userpath);

bool checkValidity(char *path);
// Return 'g' if success, otherwise return reqired type
char checkVar(char type, char *variable);
bool isDigit(char input);

int countKeyframes(char *scenePath);
bool isKeyframeValid(char *scenePath);

#endif
