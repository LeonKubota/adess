#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define COLOR_BOLD  "\e[1m"
#define COLOR_RESET "\e[m"

#define MAX_OPT_COUNT 16 // Maximum number of values, must be larger than length of g_opts
#define MAX_VAL_COUNT 2 // Maximum number of values

// Include
#include "parse.h"

// Self
#include "main.h"

// Global variables
bool g_debug = 1; // 0 for normal, 1 for debug
char *g_optslist = "hn:o"; // Options ':' mean that option requires a value, otherwise values are errors

int g_maxoptcount = MAX_OPT_COUNT;
int g_maxvalcount = MAX_VAL_COUNT;

bool g_opts[MAX_OPT_COUNT]; // Array of if values are on
char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT]; // Holds values (first index corresponds to option index)

int main(int argc, char **argv) {

	// Initilize options (won't work otherwise)
	for (int i = 0; i < g_maxoptcount; i++) {
		g_opts[i] = false;
	}
	
	// Initilize values
	for (int x = 0; x < g_maxoptcount; x++) {
		for (int y = 0; y < g_maxvalcount; y++) {
			g_vals[x][y] = "";
		}
	}

	parse(argc, argv);
}	
