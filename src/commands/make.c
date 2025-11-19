#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "commands/command.h"
#include "commands/make.h"

bool g_opts[MAX_OPT_COUNT];
char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int make(char *arg) {
	d_showInput("make", arg);
	
	// If there is an argument
	if (arg) {
		// project
		if (strcmp(arg, "project") == 0) {
			printf("project\n");
		}
		// engine
		else if (strcmp(arg, "engine") == 0) {
			printf("engine\n");
		}
		// scene
		else if (strcmp(arg, "scene") == 0) {
			printf("scene\n");
		}
		// key
		else if (strcmp(arg, "key") == 0) {
			printf("key\n");
		}
		// default (error)
		else {
			e_fatal("incorrect component '%s'\n", arg);
			return 1;
		}
	} else {
		e_fatal("no component provided\n");
		return 1;
	}
	return 0;
}
