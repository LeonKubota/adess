#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DCOL "\x1b[34m"
#define RCOL "\x1b[0m"

#include "commands/command.h"

extern bool g_debug;
extern char *g_options;

void d_showInput(char *name, char *arg, bool *opts) {
	if (g_debug) {
		// Print command
		printf(DCOL "[DEBUG]" RCOL "\tcommand:\t%s\n", name);

		// Print argument
		if (arg) {
			printf(DCOL "[DEBUG]" RCOL "\targument:\t%s\n", arg);
		} else {
			printf(DCOL "[DEBUG]" RCOL "\targument:\t(null)\n");
		}

		// Print options
		int optssize = sizeof(*opts) / sizeof(opts[0]);

		if (!opts) {
			printf(DCOL "[DEBUG]" RCOL "\toptions:\t(null)\n");
		} else if (optssize == 0) {
			printf(DCOL "[DEBUG]" RCOL "\toptions:\t(null)\n");
		} else {
			printf(DCOL "[DEBUG]" RCOL "\toptions:\t");
			d_listOptions(opts);
			printf("\n");
		}
	}
}

void d_listOptions(bool *opts) {
	int size = 0;
	bool empty = true;
	while (true) {
		if (g_options[size] == 0) {
			break;
		}
		if (!(opts[size] == 0)) {
			empty = false;
		}
		size++;
	}
	if (empty) {
		printf("(null)");
	} else {
		for (int i = 0; i < size; i++) {
			if (opts[i]) {
				printf("%c", g_options[i]);
			}
		}
	}
}
