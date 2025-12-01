#include <stdbool.h>

#ifndef HELP_H 
#define HELP_H

#include "main.h"

int help(char **args);
void defaultHelp();

void showUsage(char *name, bool components, char *opts);
void showOptions();
void showDescription(char *description);
char *getLongOpt(char opt);

#endif
