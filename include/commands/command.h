#include <stdbool.h>

#ifndef COMMAND_H
#define COMMAND_H

#include "main.h"

void d_showInput(char *name, char **args);
void d_listArgs(char **args);
void d_listOptions(void);
void d_listValues(void);
int makeFile(char *path);
int appendLine(char *path, const char *format, ...);

#endif
