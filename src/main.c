#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include <time.h>

// Include
#include "parse.h"

// Self
#include "main.h"

// Global variables
bool g_debug = false; // 'false' for normal, 'true' for debug
char *g_optslist = "hn.d.eap"; // Options

// DO NOT CHANGE THE ORDER, YOU WILL BREAK EVERYTHING (I AM STUPID)

/*
	.	single value expected
	:	multiple values expected
0	h	help
1	n	name
3	d	directory
5	e	empty
6	a	all
7	p	preview
*/

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
