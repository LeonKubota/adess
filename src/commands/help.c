#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "commands/command.h"
#include "commands/help.h"

int help(char *arg, bool *opts) {
	d_showInput("help", arg, opts);

	// If there is an argument, print argument specific help
	if (arg) {
		// make (mk)
		if (strcmp(arg, "make") == 0 || strcmp(arg, "mk") == 0) {
			char *availablecomps[] = {"project", "engine", "scene", "key", NULL};
			char *availableopts[] = {"-n, --name", "set the name", "-d, --dir", "set the directory", NULL};
			showHelp("make", "<component> [options]", "use the 'make' command to create a component.", availablecomps, availableopts);
		} else {
			printf("adess: no help available for argument '%s'\n", arg);
		}
	} else {
		defaultHelp();
	}
	return 0;
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

void showHelp(char *name, char *usage, char *desc, char **comps, char **options) {
	printf("usage: adess %s %s\n\ndescription: %s\n\n", name, usage, desc);

	// Print components inline (if there are any)
	if (comps) {
		printf("components: ");

		int i = 0;
		while (true) {
			if (comps[i] == NULL) {
				break;
			}
			printf("%s", comps[i]);
			// So it doesn't end like this: "component1, component2, component3, "
			if (!(comps[i + 1] == NULL)) {
				printf(", ");
			}
			i++;
		}
		printf("\n");
	}

	// Print options in this format:
	// -t, -time    show execution time
	// even index = option, odd index = description
	if (options) {
		printf("\n");
	
		int i = 0;
		while (true) {
			if (options[i] == NULL) {
				break;
			}
			printf("\t%s\t%s\n", options[i], options[i + 1]);
			i += 2;
		}
	}
}
