#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DCOL "\x1b[1;35m"
#define RCOL "\x1b[0m"

#include "commands/command.h"

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

void d_listValues() {
	bool empty = true;
	for (int opt = 0; g_optslist[opt] != '\0'; opt++) {
		// Option
		for (int val = 0; val < MAX_VAL_COUNT; val++) {
			// Value
			if (strcmp(g_vals[opt][val], "") != 0) {
				printf("%c: '%s', ", g_optslist[opt], g_vals[opt][val]);
				empty = false;
			}
		}
	}
	// Print '(null)' if vals are empty
	if (empty) {
		printf("(null)");
	}
}
