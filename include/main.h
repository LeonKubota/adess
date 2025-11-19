#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

#define MAX_OPT_COUNT 16
#define MAX_VAL_COUNT 2

extern bool g_debug;
extern char *g_optslist;

extern int g_maxoptcount;
extern int g_maxvalcount;

extern bool g_opts[MAX_OPT_COUNT];
extern char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int main(int argc, char **argv);

#endif
