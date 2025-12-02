#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "commands/command.h"
#include "utils.h"
#include "commands/help.h"

int help(char **args) {
	d_showInput("help", args);

	if (args == NULL) {
		defaultHelp();
		return 0;
	}

	// If there is an argument, print argument specific help
	if (args[2] && !args[3]) {
		// make
		if (strcmp(args[2], "make_project") == 0) {
			showUsage("make_project", true, "hn.d:e");
			showDescription("the make_project command is used for creating a adess project\n");
		} 
		// help help
		else if (strcmp(args[2], "help") == 0) {
		
		} else {
			n_print("adess: no help available for argument '%s'\n", args[2]);
		}
	} else {
		e_fatal("too many arguments for command 'help'\n");
		return 1;
	}
	return 0;
}

void showUsage(char *name, bool components, char *opts) {
	// Print first part
	printf("usage: adess %s ", name);	
	
	// Print components (if applicable)
	if (components) {
		printf("<component> ");
	}

	int i = 0;
	// Print options
	while (opts[i] != '\0') {
		// Skip extra characters
		if (!((opts[i] == '.') || (opts[i] == ':'))) {
			// Single value
			if (opts[i + 1] == '.') {
				printf("[-%c | --%s=<vals>] ", opts[i], getLongOpt(opts[i]));
			}
			// Multiple values
			else if (opts[i + 1] == ':') {
				printf("[-%c | --%s=[<vals>]] ", opts[i], getLongOpt(opts[i]));
			}
			// No values
			else {
				printf("[-%c | --%s] ", opts[i], getLongOpt(opts[i]));
			}
		}

		// New line every 3 lines
		if ((i + 1)%4 == 0) {
			printf("\n");
		}
		// Make equal length
		if (i > 3) {
			printf("       "); // length of "usage: "
			printf("      "); // length of "adess "
			for (int n = 0; name[n] != '\0'; n++) {
				printf(" ");
			}
			printf(" "); // there is a space after the name
			if (components) {
				printf("            "); // length of "<component> "
			}
		}
		i++;
	}
	printf("\n\n");
}

// TODO
void showOptions() {
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

void defaultHelp() {
	// Print header
	printf("usage: adess <command> [<options>]\n\n");
	printf("Adess - artist directed engine sound synthetizer\n\n");
	printf("These are some Adess commands (see 'adess help <command>' for more)\n");

	// Commands
	printf("\tmake_project\tMake a new project\n");
	printf("\trender\tRender a scene\n");
	
	// Ending, suggest other info
}
