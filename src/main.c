#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "parse.h"
#include "commands/command.h"

// Commands
#include "commands/help.h"

bool g_debug = 1;
char *g_options = "hn:o"; // If over 8 long, edit MAX_OPT_COUNT in parse.c, technically should be ordered by usage frequency

/*
-h, --help = help with a command
-n, --name = name (use with make command)
*/

int main(int argc, char **argv) {
	// If there is no argument (the input is nothing or a flag)
	if (countArgs(argc, argv) == 1) {
		// --help (-h)
		if (!argv[1] || (strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
			help(NULL, NULL, NULL);
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
			return 1;
		}
	}
	
	// Check how many arguments are given and decide what to do next
	switch (countArgs(argc, argv)) {
		case 2:
		case 3:
			return parseCommand(argc, argv);
		default:
			printf("fatal: too many arguments\n");
			return 1;
	}
}	
