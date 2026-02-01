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
        if (strcmp(args[2], "guide") == 0) {
            showUsage("guide", "<topic>", "");
            showDescription("see the guide for a better understanding of adess\n");
        }
		// make_project
        else if (strcmp(args[2], "make_project") == 0) {
			showUsage("make_project", NULL, "hn.d:e");
			showDescription("the make_project command is used for creating an adess project\n");
			explainOptions("hn.d.e");
		} else if (strcmp(args[2], "make_scene") == 0) {
			showUsage("make_scene", NULL, "hn.d:e");
			showDescription("the make_scene command is used for creating a scene\n");
			explainOptions("hn.d.e");
		} else if (strcmp(args[2], "make_engine") == 0) {
			showUsage("make_engine", NULL, "hn.d:e");
			showDescription("the make_engine command is used for creating an engine\n");
			explainOptions("hn.d.e");
		} else if (strcmp(args[2], "render") == 0) {
			showUsage("render", "<scene>" , "n.ap");
			showDescription("the render command is used for rendering a scene\n");
			explainOptions("n.ap");
		} else if (strcmp(args[2], "help") == 0) {
			defaultHelp();
			return 0;
		} else {
			e_fatal("no help available for command '%s', no such command exists\n", args[2]);
			return 1;
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

void explainOptions(char *options) {
	printf("\nAvailable options:\n");

	while (options[0] != '\0') {
		printf("\t[-%c] ", options[0]);

		// Description of the option
		if (options[0] == 'h') {
			printf("help -  prints a help message about the command");
		} else if (options[0] == 'n') {
			printf("name - adds a name");
		} else if (options[0] == 'd') {
			printf("directory - specifies a directory");
		} else if (options[0] == 'e') {
			printf("empty - creates without default values");
		} else if (options[0] == 'a') {
			printf("all - renders every scene");
		} else if (options[0] == 'p') {
			printf("preview - renders in preview mode (quicker)");
		}

		if (options[1] == '.') {
			printf(", this option accepts a single value\n");
			options += 2;
			continue;
		} else if (options[1] == ':') {
			printf(", this option accepts multiple values\n");
			options += 2;
			continue;
		}

		printf("\n");
		options++;
	}
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
	
	// Ending, suggest other info
	printf("\nTo see an in depth guide of Adess, see 'adess guide'\n");
}
