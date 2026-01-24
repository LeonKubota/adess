#include <stdbool.h>

#include "files/extract.h"
#include "main.h"

#ifndef MAKE_PROJECT_H
#define MAKE_PROJECT_H 

int make_project(char **args);

void appendProjectDefault(char *path);
int makeDirectory(char *path);
int makeFile(char *path);
int appendLine(char *path, const char *format, ...);

#endif
