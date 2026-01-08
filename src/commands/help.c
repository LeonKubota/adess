#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "commands/command.h"
#include "utils.h"
#include "commands/help.h"

int help(char **args) {
	d_showInput("help", args);

	if (args == NULL || args[2] == NULL) {
		defaultHelp();
		return 0;
	}

	// If there is an argument, print argument specific help
	if (args[2]) {
		// make_project
		if (strcmp(args[2], "make_project") == 0) {
			showUsage("make_project", NULL, "hn.d:e");
			showDescription("the make_project command is used for creating an adess project, setting the name by using [-n] is required.\nIf you want the created project to be empty, use [-e].\n");
		} 
		else if (strcmp(args[2], "make_scene") == 0) {
			showUsage("make_scene", NULL, "hn.d:e");
			showDescription("the make_scene command is used for creating a scene, setting the name by using [-n] is required.\nIf you want the created scene to be empty, use [-e].\n");
		}
		else if (strcmp(args[2], "make_engine") == 0) {
			showUsage("make_engine", NULL, "hn.d:e");
			showDescription("the make_engine command is used for creating an engine, setting the name by using [-n] is required.\nIf you want the created engine to be empty, use [-e].\n");
		}
		else if (strcmp(args[2], "render") == 0) {
			showUsage("render", "<scene>" , "n.af");
			showDescription("the render command is used for rendering a scene, you can set the name of the output file by using [-n].\nFor rendering all the scenes, use [-a]. You can also use [-p] to preview a scene (skip expensive post processing)\n");
		}
		else if (strcmp(args[2], "help") == 0) {
		
		} else {
			e_fatal("no help available for argument '%s'\n", args[2]);
		}
	} else {
		e_fatal("too many arguments for command 'help'\n");
		return 1;
	}
	return 0;
}

void showUsage(char *name, char *desc, char *opts) {
	// Print first part
	printf("usage: adess %s ", name);	
	
	// Print desc
	if (desc != NULL) {
		printf("%s ", desc);	
	}

	int i = 0;
	// Print options
	while (opts[i] != '\0') {
		// Skip extra characters
		if (!((opts[i] == '.') || (opts[i] == ':'))) {
			// Single value
			if (opts[i + 1] == '.') {
				printf("[-%c <val>]", opts[i]);
			}
			// Multiple values
			else if (opts[i + 1] == ':') {
				printf("[-%c [<vals>]]", opts[i]);
			}
			// No values
			else {
				printf("[-%c]", opts[i]);
			}
		}

		// New line every 8 options
		if ((i + 1)%9 == 0) {
			printf("\n");
		}
		// Make equal length
		if (i > 8) {
			printf("       "); // length of "usage: "
			printf("      "); // length of "adess "
			for (int n = 0; name[n] != '\0'; n++) {
				printf(" ");
			}
			printf(" "); // there is a space after the name
		}
		i++;
	}
	printf("\n\n");
}

// TODO
void showOptions(void) {
}

void showDescription(char *description) {
	printf("description: %s", description);
}

char *getLongOpt(char opt) {
	if (opt == 'h') {
		return "help";
	} else if (opt == 'n') {
		return "name";
	} else if (opt == 'd') {
		return "directory";
	} else if (opt == 'e') {
		return "empty";
	}

	return "\n\n\ngetLongOpt in help.c failed";
}

void defaultHelp(void) {
	// Print header
	printf("usage: adess <command> [<options>]\n\n");
	printf("Adess - artist directed engine sound synthetizer\n\n");
	printf("These are some Adess commands (see 'adess help <command>' for more)\n");

	// Commands
	printf("\tmake_project\tMake a new project\n");
	printf("\tmake_scene\tMake a new scene\n");
	printf("\tmake_engine\tMake a new engine\n");
	printf("\trender\t\tRender a scene\n");
	
	// Ending, suggest other info
}
