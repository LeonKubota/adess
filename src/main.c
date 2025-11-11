#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "parse.h"
#include "commands/command.h"

// Commands
#include "commands/help.h"

bool g_debug = 1;
char *g_options = "ilrf"; // If over 8 long, edit MAX_OPT_COUNT in parse.c

int main(int argc, char **argv) {
	// If there is no argument (the input is nothing or a flag)
	if (countArgs(argc, argv) == 1) {
		// --help (-h)
		if (!argv[1] || (strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
			help(NULL, NULL);
			return 0;
		}

		// --version (-v)
		else if ((strcmp(argv[1], "--version") == 0) || (strcmp(argv[1], "-v") == 0)) {
			printf("Version: HAS TO BE MADE\n");
			return 0;
		}
		// default (unknown option)	
		 else {
			printf("adess: unknown option\n");
			return 0;
		}
	}
	
	// Check how many arguments are given and decide what to do next
	switch (countArgs(argc, argv)) {
		case 2:
			return parseCommand(argc, argv);
		case 3:
			parseCommand(argc, argv);
			return 0;
		default:
			printf("fatal: too many arguments\n");
			return 0;
	}

	return 0;
}	
