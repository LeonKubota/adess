#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "commands/command.h"
#include "commands/make.h"

int make(char *arg, bool *opts, char **vals) {
	d_showInput("make", arg, opts);
	
	// If there is an argument
	if (arg) {
		// project
		if (strcmp(arg, "project\n") == 0) {
			printf("project");
		}
		// engine
		else if (strcmp(arg, "engine\n") == 0) {
			printf("engine");
		}
		// scene
		else if (strcmp(arg, "scene\n") == 0) {
			printf("scene");
		}
		// key
		else if (strcmp(arg, "key\n") == 0) {
			printf("key");
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
