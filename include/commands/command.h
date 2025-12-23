#include <stdbool.h>

#ifndef COMMAND_H
#define COMMAND_H

#include "main.h"

void d_showInput(char *name, char **args);
void d_listArgs(char **args);
void d_listOptions(void);
void d_listValues(void);
char **getValue(char option);

#endif
