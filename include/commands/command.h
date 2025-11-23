#include <stdbool.h>

#ifndef COMMAND_H
#define COMMAND_H

#include "main.h"

void d_showInput(char *name, char *arg);
void d_listOptions();
void d_listValues();
char **getValue(char option);

#endif
