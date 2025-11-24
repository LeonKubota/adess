#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "commands/command.h"
#include "commands/help.h"

int help(char *arg) {
	d_showInput("help", arg);

	// If there is an argument, print argument specific help
	if (arg) {
		// make
		if (strcmp(arg, "make") == 0) {
			showUsage("make", true, "hn.d:e");
			showDescription("the make command is for creating adess components\n");
		} 
		// help help
		else if (strcmp(arg, "help") == 0) {
		} else {
			printf("adess: no help available for argument '%s'\n", arg);
		}
	} else {
		defaultHelp();
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
	printf("usage: adess <command> <component> [<options>]\n\n");
	printf("Adess - artist directed engine sound synthetizer\n\n");
	printf("These are some Adess commands (see 'adess help <command>' for more)\n");

	// Commands
	printf("\tmake\tMake a new component\n");
	printf("\tedit\tEdit an existing component\n");
	printf("\tview\tView the data of a component\n");
	printf("\tremove\tRemove a component\n");
	printf("\trender\tRender a scene\n");
	
	// Adess components
	printf("\nThese are the components of Adess (see 'adess help components' for more)\n");
	
	printf("\tproject\tProjects are directories that contain everything Adess needs.\n");
	printf("\tengine\tEngine files contain information about an engines characteristics.\n");
	printf("\tscene\tScenes hold keyframes and can be rendered into actual sound files.\n");
	printf("\tkey\tKeyframes are used for animation, they belong to scenes.\n");

	// Ending, suggest other info
	printf("\nSee 'adess <command> -h' or 'adess help <command>' for help with a specific command.\nTry 'adess help <component>' for help with a specific component. See 'adess help guide' for a deeper guide aboud Adess.\n"); 
}
