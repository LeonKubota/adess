#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DCOL "\x1b[34m"
#define RCOL "\x1b[0m"

#include "commands/command.h"

bool g_opts[MAX_OPT_COUNT];
char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

void d_showInput(char *name, char *arg) {
	if (g_debug) {
		// Print name of command (name)
		printf(DCOL "[DEBUG]" RCOL "\tcommand:\t%s\n", name);

		// Print argument (arg)
		if (arg) {
			printf(DCOL "[DEBUG]" RCOL "\targument:\t%s\n", arg);
		} else {
			printf(DCOL "[DEBUG]" RCOL "\targument:\t(null)\n");
		}

		// Print options (g_opts)
		printf(DCOL "[DEBUG]" RCOL "\toptions:\t");
		d_listOptions();
		printf("\n");

		// Print values (g_vals)
		printf(DCOL "[DEBUG]" RCOL "\tvalues:\t\t");
		d_listValues();
		printf("\n");
	}
}

void d_listOptions() {
	int size = 0;
	bool empty = true;
	while (true) {
		if (g_optslist[size] == 0) {
			break;
		}
		if (!(g_opts[size] == 0)) {
			empty = false;
		}
		size++;
	}
	if (empty) {
		printf("(null)");
	} else {
		for (int i = 0; i < size; i++) {
			if (g_opts[i]) {
				printf("%c", g_optslist[i]); }
		}
	}
}

// TODO
void d_listValues() {
	bool empty = true;
	//int i = 0;

	/*
	while (vals[i][0] != NULL) {
		// Only for non-empty values
		if (strcmp(vals[i], "") != 0) {
			//printf("i, %s; ", vals[i]);

			int n = 0; // Is not defined at start because there may be no values
			while (vals[i][n] != '\0') {
				//print a single character
				printf("%c", vals[i][n]);
				n++;
			}
			//printf("n: %i", n);
			empty = false;
		}
		i++;
	}
	*/
	if (empty) {
		printf("(null)");
	}
}
