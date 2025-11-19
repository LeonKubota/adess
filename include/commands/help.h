#include <stdbool.h>

#ifndef HELP_H 
#define HELP_H

#include "main.h"

int help(char *arg);
void showHelp(char *name, char *usage, char *desc, char **comps, char **options);
void defaultHelp();

#endif
