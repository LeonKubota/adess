#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "commands/command.h"
#include "commands/make.h"

int make(char *arg, bool *opts, struct Values *vals) {
	d_showInput("make", arg, opts);
	
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
			printf("fatal: unknown component '%s'\n", arg);
			return 1;
		}
	} else {
		printf("fatal: no component provided\n");
		return 1;
	}
	return 0;
}
