#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

// TODO - change to 1 later
#define VERSION "0.0.1"

#define MAX_ARG_COUNT 16 // This is based on the user
#define MAX_OPT_COUNT 8
#define MAX_VAL_COUNT 2

#define FLOAT_FAIL -69420.42069f // Hopefully no user enters 69420.42069f as their float
#define INT_FAIL -676967 // I hope noone uses this

#ifdef _WIN32
#include <windows.h>
#include <direct.h> // for _getcwd
#define PATH_SEPARATOR "\\" // Windows' weird things (idiot Bill Gates)
#define getcwd _getcwd // so that its the same later
#else
#include <unistd.h> // for gecwd
#define PATH_SEPARATOR "/" // for sane people
#endif

struct Keyframe {
	float keytime;
	int rpm;
	//float load;
};

extern bool g_debug;
extern char *g_optslist;

extern int g_maxoptcount;
extern int g_maxvalcount;

extern bool g_opts[MAX_OPT_COUNT];
extern char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int main(int argc, char **argv);

#endif
