#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Utils
#include "utils.h"

// Self
#include "parse.h"

// Commands
#include "commands/help.h"
#include "commands/make.h"

bool g_debug;
char *g_optslist;

int g_maxoptcount;
int g_maxvalcount;

bool g_opts[MAX_OPT_COUNT];
char *g_vals[MAX_OPT_COUNT][MAX_VAL_COUNT];

int parse(int argc, char **argv) {
	// If there is no argument (the input is nothing or a flag)
	if (countArgs(argc, argv) == 1) {
		// --help (-h)
		if (!argv[1] || (strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
			help(NULL);
			return 0;
		}
		// --version (-v)
		else if ((strcmp(argv[1], "--version") == 0) || (strcmp(argv[1], "-v") == 0)) {
			printf("Version: HAS TO BE MADE\n");
			return 0;
		}
		// default (unknown option)	
		 else {
        	e_fatal("unknown option '%s'\n", argv[1]);
			return 1;
		}
	}
	
	// Check how many arguments are given and decide what to do next
	switch (countArgs(argc, argv)) {
		case 2:
		case 3:
			return parseCommand(argc, argv);
		default:
            e_fatal("too many arguments\n");
			return 1;
	}
}

int parseCommand(int argc, char **argv) {
	int argscount = countArgs(argc, argv) - 1;

	// Check if first argument is a real argument or option
	char *arg = NULL;
	if (argv[2]) {
		if (!(argv[2][0] == '-')) {
			arg = argv[2];
		}
	}
	
	// help
	if (strcmp(argv[1], "help") == 0) {
		return execute(help, argc, argv, argscount, "", arg);
	}
	// make
	else if ((strcmp(argv[1], "make")) == 0) {
		return execute(make, argc, argv, argscount, "no", arg);
	}
	// edit
	else if ((strcmp(argv[1], "edit")) == 0) {
		printf("Edit command\n");
	}
	// view
	else if ((strcmp(argv[1], "view")) == 0) {
		printf("View command\n");
	}
	// remove (rm)
	else if (((strcmp(argv[1], "remove")) == 0) || ((strcmp(argv[1], "rm")) == 0)) {
		printf("Remove command\n");
	}
	// default (unknown command)
	else {
		printf("adess: '%s' is not an adess command. See 'adess --help'\n", argv[1]);
		return 1;
	}
	return 1;
}

int parseOptions(char **argv) {
    // Can start with 1 (skip over first argument / '-')
    int i = 1;
    int n = 1;
    bool valexpected = false;

    // For each argument (each string in input)
	while (!(argv[i] == NULL)) {
		// Detect options
		if (argv[i][0] == '-') {

			// Long options (eg. "--input") TODO
            if (argv[i][1] == '-') {
				printf("Long option\n");
                return 0;
            }

			n = 1;
            // Short options (eg. "-i")
            while (argv[i][n]) {
            	// If the option is valid and not ':'
                if (simpleIsValid(argv[i][n]) && argv[i][n] != ':') {
					// If valexpected is set to 'true' (when previous opt requires val (':'))
               		if (valexpected) {
                    	e_fatal("value expected for options '%s'\n", "option (TODO)");
                        return 1;
                    }
					// Set valexpected to true if value is expected
                    valexpected = valExpected(argv[i][n]);
                	g_opts[optIndex(argv[i][n])] = 1;
                }
                // If invalid option
                else {
					printf("test!");
					e_fatal("unknown option '%c'\n", argv[i][n]);
                    return 1;
           		}
				n++;
       		}
		}

        // Detect values
        if (valexpected) {
			// Iterate through arguemnts after last argument until option or NULL
			n = i;
			while (argv[n][1] != '-' && argv[n] != NULL) {
				printf("value #%i: %s\n", n, argv[n]);
				n++;
			}
			valexpected = 0;
        }

        // Check if there are undefined values at the end of parsing options
        if (valexpected) {
			e_fatal("value expected for option '%s'\n", "test valexpected");
            return 1;
        }
        i++;
    }
	return 0;
}

bool simpleIsValid(char opt) {
    int i = 0;
    // Look through the g_optslist and return true if opt is found
    while(!(g_optslist[i] == 0)) {
        if (g_optslist[i] == opt) {
            return 1;
        }
        i++;
    }
    return 0;
}

bool valExpected(char opt) {
    int i = 0;
    while(!(g_optslist[i] == 0)) {
        if (g_optslist[i] == opt) {
            if (g_optslist[i + 1] == ':') {
                return 1;
            }
        }
        i++;
    }
    return 0;
}

// returns index if there is an option, returns -1 if no option
int optIndex(char opt) {
    int i = 0;
    while(!(g_optslist[i] == 0)) {
        if (g_optslist[i] == opt) {
            return i;
        }
        i++;
    }
    return -1;
}

int execute(int (*command)(char *arg), int argc, char **argv, int argscount, char *accepted, char *arg) {
	if (parseOptions(argv + argscount) == 0) {
		if ((unacceptedOptions(accepted)) == -1) {
			command(arg);
			return 0;
		} else {
			e_fatal("unknown option '%c'\n", g_optslist[unacceptedOptions(accepted)]);
			help(argv[1]);
			return 1;
		}
	} else {
		return 1;
	} 
}

// Returns unknown options index in *g_optslist or -1 if successful
int unacceptedOptions(char *accepted) {
	int i = 0;
	while (!(g_optslist[i] == 0)) {
        // If the option is on
		if (g_opts[i] == 1) {
			int n = 0;
			while (true) {
                if (g_optslist[i] == accepted[n] || g_optslist[i] == 'h') {
                    break;
                }
				// If iterated through entire accepted array and didn't find it
				if (accepted[n] == 0) {
					return i;
				}
				n++;
			}
			
		}
		i++;
	}
	return -1;
}

int countArgs(int argc, char **argv) {
	for (int i = 0; i < argc; i++) {
		if (argv[i][0] == '-') {
			return i;
		}
	}
	return argc;
}
