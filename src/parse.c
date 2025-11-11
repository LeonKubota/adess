#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "parse.h"

// Commands
#include "commands/command.h"
#include "commands/help.h"
#include "commands/make.h"

#define MAX_OPT_COUNT 8
#define MAX_VAL_COUNT 64
#define MAX_VAL_LENGTH 32

extern char g_debug;
extern char *g_options;

int parseCommand(int argc, char **argv) {
	int argscount = countArgs(argc, argv) - 1;

	bool options[MAX_OPT_COUNT] = {false};

    char *values[MAX_VAL_COUNT];

    // Initialize values array
    for (int i = 0; i < MAX_VAL_COUNT; i++) {
        values[i] = 0;
    }

	// Check if first argument is a real argument or option
	char *arg = NULL;
	if (argv[2]) {
		if (!(argv[2][0] == '-')) {
			arg = argv[2];
		}
	}
	
	// Arguments for the execute function pointer to function, int argc, char **argv, bool *options, int argscount, char *accepted, char *arg

	// help
	if (strcmp(argv[1], "help") == 0) {
		return execute(help, argc, argv, options, values, argscount, "", arg);
	}
	// make (mk)
	else if (((strcmp(argv[1], "make")) == 0) || ((strcmp(argv[1], "mk")) == 0)) {
		return execute(make, argc, argv, options, values, argscount, "n", arg);
	}
	// edit (ed)
	else if (((strcmp(argv[1], "edit")) == 0) || ((strcmp(argv[1], "ed")) == 0)) {
		printf("Edit command\n");
		return execute(make, argc, argv, options, values, argscount, "n", arg);
	}
	// view (vw)	
	else if (((strcmp(argv[1], "view")) == 0) || ((strcmp(argv[1], "vw")) == 0)) {
		printf("View command\n");
		return execute(make, argc, argv, options, values, argscount, "n", arg);
	}
	// remove (rm)
	else if (((strcmp(argv[1], "remove")) == 0) || ((strcmp(argv[1], "rm")) == 0)) {
		printf("Remove command\n");
		return execute(make, argc, argv, options, values, argscount, "n", arg);
	}
	// default (unknown command)
	else {
		printf("adess: '%s' is not an adess command. See 'adess --help'\n", argv[1]);
		return 1;
	}
	return 1;
}

int parseOptions(bool *options, char **values, char **argv) {
    // Can start with 1 (skip over first argument / '-')
    int i = 1;
    int n = 1;
    bool valexpected = false;

    // For each argument (each string in input)
    while (!(argv[i] == NULL)) {
        n = 1;

        // Detect options
        if (argv[i][0] == '-') {
            // Long options (eg. "--input")
            if (argv[i][1] == '-') {
                printf("Long option\n");
                return 0;
            }
            // Short options (eg. "-i")
            while ((argv[i][n])) {
                // If the option is valid (also check for ':', which is used to signify an option that requires a value
                if (simpleIsValid(argv[i][n]) && argv[i][n] != ':') {
                    if (valexpected) {
                        printf("fatal: value expected for option '%s'\n", "option (TODO, last option in stack)");
                        return 1;
                    }
                    valexpected = valExpected(argv[i][n]);
                    options[optIndex(argv[i][n])] = 1;
                }
                // If invalid option
                else {
                    printf("fatal: unknown option '%c'\n", argv[i][n]);
                    d_printFail(strcat(argv[i], " is not a valid option"));
                    return 1;
                }
                n++;
            }
        }

        // Detect values
        printf("valexpected(vals): %d\n", valexpected);
        if (valexpected) {
            printf("trying to add to values\n");
            sprintf(values[0], "%d", optIndex(argv[i][n]));
            valexpected = false;
        } else {
            printf("fatal: unexpected value '%s' for option '%s'\n", "value", "option (TODO, last option in option stack)");
            return 1;
        }

        // Check if there are undefined values
        if (valexpected) {
            printf("end check\n");
            printf("fatal: value expected for option '%s'\n", "option (TODO, last option in the option stack)");
            return 1;
        }
        i++;
    }
	return 0;
}

bool simpleIsValid(char opt) {
    int i = 0;
    // Look through all g_options and return true if opt is found
    while(!(g_options[i] == 0)) {
        if (g_options[i] == opt) {
            return 1;
        }
        i++;
    }
    return 0;
}

bool valExpected(char opt) {
    int i = 0;
    while(!(g_options[i] == 0)) {
        if (g_options[i] == opt) {
            if (g_options[i + 1] == ':') {
                return 1;
            }
        }
        i++;
    }
    return 0;
}

int optIndex(char opt) {
    int i = 0;
    while(!(g_options[i] == 0)) {
        if (g_options[i] == opt) {
            return i;
        }
        i++;
    }
    return -1;
}

int execute(int (*command)(char *arg, bool *opts, char **values), int argc, char **argv, bool *options, char **values, int argscount, char *accepted, char *arg) {
	if (parseOptions(options, values, argv + argscount) == 0) {
        // print values
        int i = 0;
        while (!(values[i] == 0)) {
            printf("%s, %s", values[i], values[i]);
            i++;
        }

		if ((unacceptedOptions(options, accepted)) == -1) {
			command(arg, options, values);
			return 0;
		} else {
			printf("fatal: unknown option '%c'\n", g_options[unacceptedOptions(options, accepted)]);
			help(argv[1], NULL, values);
			d_printFail(strcat(argv[1], " value not accepted (unacceptedOptions)"));
			return 1;
		}
	} else {
		d_printFail(strcat(argv[1], " (parseCommand)"));
		return 1;
	} 
}

// Returns unknown options index in *g_options or -1 if successful
int unacceptedOptions(bool *options, char *accepted) {
	int i = 0;
	while (!(g_options[i] == 0)) {
        // If the option is on
		if (options[i] == 1) {
			int n = 0;
			while (true) {
                if (g_options[i] == accepted[n] || g_options[i] == 'h') {
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

void d_printFail(char *msg) {
	if (g_debug) {
		printf("\x1b[34m""[DEBUG]""\x1b[0m""\tfailed: %s\n", msg);
	}
}
