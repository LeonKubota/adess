#include <stdbool.h>

#ifndef HELP_H 
#define HELP_H

#include "main.h"

int help(char **args);
void defaultHelp(void);

void showUsage(char *name, char *desc, char *opts);
void explainOptions(char *options);
void showDescription(char *description);

#endif
