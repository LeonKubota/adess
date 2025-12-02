#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

// TODO
#define VERSION "0.0.1"

#define MAX_ARG_COUNT 16 // This is based on the user
#define MAX_OPT_COUNT 8
#define MAX_VAL_COUNT 2

extern bool g_debug;
extern char *g_optslist;

extern int g_maxoptcount;
extern int g_maxvalcount;

extern bool g_opts[MAX_OPT_COUNT];
extern char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int main(int argc, char **argv);

#endif
