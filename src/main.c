#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define COLOR_BOLD  "\e[1m"
#define COLOR_RESET "\e[m"

// Include
#include "parse.h"

// Self
#include "main.h"

// Global variables
bool g_debug = true; // 'false' for normal, 'true' for debug
char *g_optslist = "hn:d:"; // Options ':' mean that option requires a value, otherwise values are errors

/*
0	h	help
1	n	name
3	d	directory
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

/*
void printValues() {
    for (int i = 0; i < MAX_OPT_COUNT; i++) {
		printf("%c\t", g_optslist[i]);
        for (int j = 0; j < MAX_VAL_COUNT; j++) {
            // Check if the string is not NULL to avoid printing garbage
            if (g_vals[i][j] != NULL) {
				printf("single value"
                printf("y[%i]\t'%s'\t", j, g_vals[i][j]);
            }
        }
        printf("\n");  // New line after each row
    }
}
*/
