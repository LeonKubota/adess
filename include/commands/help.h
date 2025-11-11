#include <stdbool.h>

#ifndef HELP_H 
#define HELP_H

int help(char *arg, bool *opts);
void showHelp(char *name, char *usage, char *desc, char **comps, char **options);
void defaultHelp();

#endif
