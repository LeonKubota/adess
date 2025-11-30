#ifndef CHECK_H
#define CHECK_H

#include "utils.h"

bool checkFileExistsIn(char *path, char *filename);
bool checkFileExists(char *path);
char *findProjectFile(char *path);
char *getCurDirectory(char *userpath);

bool checkValidity(char *path);
bool checkVar(char type, char *variable);

#endif
