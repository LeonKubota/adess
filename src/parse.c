#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Utils
#include "utils.h"

// Self
#include "parse.h"

// Commands
#include "commands/help.h"
#include "commands/make_project.h"
#include "commands/make_scene.h"
#include "commands/render.h"

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
			version(); // Version is defined in main.h
			return 0;
		}
		// default (unknown option)	
		 else {
        	e_fatal("unknown option '%s'\n", argv[1]);
			return 1;
		}
	}
	
	// If there are arguments
	return parseCommand(argc, argv);
}

int parseCommand(int argc, char **argv) {
	int argscount = countArgs(argc, argv) - 1;
	
	// Add just the arguments into args
	int i = 0;
	char *args[MAX_ARG_COUNT];

	while (argv[i] != NULL) {
		if (argv[i][0] != '-') {
			args[i] = argv[i];
		} else {
			args[i] = NULL;
			break;
		}

		i++;
	}
	args[i] = NULL;

	// help
	if (strcmp(argv[1], "help") == 0) {
		return execute(help, argv, args, argscount, "");
	}
	// make_project
	else if ((strcmp(argv[1], "make_project")) == 0) {
		return execute(make_project, argv, args, argscount, "hnde");
	}
	// make_scene
	else if ((strcmp(argv[1], "make_scene")) == 0) {
		return execute(make_scene, argv, args, argscount, "hne");
	}
	// render
	else if ((strcmp(argv[1], "render")) == 0) {
		return execute(render, argv, args, argscount, "anp");
	}
	// default (unknown command)
	else {
		e_fatal("'%s' is not an adess command. See 'adess --help'\n", argv[1]);
		return 1;
	}
	return 1;
}

int parseOptions(char **argv) {
    // Can start with 1 (skip over first argument / '-')
    int i = 1;
    int n = 1;
    int valexpected = 0; // 0 - no value expected; 1 - single value; 2 - multiple values
	int curoptindex = -1;

    // For each argument (each string in input)
	while (!(argv[i] == NULL)) {
		// Detect options
		if (argv[i][0] == '-') {
			n = 1;
            // Short options (eg. "-i")
            while (argv[i][n]) {
            	// If the option is valid and not ':' or '.'
                if (simpleIsValid(argv[i][n]) && (argv[i][n] != ':' || argv[i][n] != '.')) {
					// Save current options index for assigning values later
					curoptindex = optIndex(argv[i][n]);
	
					// Fail if the option is inputed twice
					if (g_opts[curoptindex]) {
						e_fatal("multiple instances of option '%c' detected\n", argv[i][n]);
						return 1;
					}

					// If valexpected is set to 'true' (when previous opt requires val (':'))
               		if (valexpected) {
                    	e_fatal("value expected for options '%s'\n", "option (TODO)");
                        return 1;
                    }

					// Set valexpected to true if value is expected
                    valexpected = valExpected(argv[i][n]);

					// Set the index in g_opts that corresponds to the options index to 1
                	g_opts[curoptindex] = true;
                }
                // If invalid option
                else {
					e_fatal("unknown option '%c'\n", argv[i][n]);
                    return 1;
           		}
				n++;
       		}
		}

        // Detect multiple values
        if (valexpected == 2) {
			// Iterate through arguemnts after last argument until option or NULL
			n = 0;
			while (argv[n + i + 1] != NULL) {
				if (argv[n + i + 1][0] == '-') {
					break;
				}

				// Save value
				g_vals[curoptindex][n] = argv[n + i + 1];
				n++;
			}
			valexpected = 0;
        }

		// Single values
		if (valexpected == 1) {
			// Check if there is a value
			if (argv[i + 1] != NULL) {
				// Check if trying to use option as a value
				if (argv[i + 1][0] == '-') {
					e_fatal("option '%s' cannot be used as value for option '%s'\n", argv[i + 1], argv[i]);
					return 1;
				}
				if (argv[i + 2] != NULL) {
					if (argv[i + 2][0] != '-') {
						e_fatal("only one value can be used with option '%s'\n", argv[i]);
						return 1;
					}
				}

				g_vals[curoptindex][0] = argv[i + 1];
			} else {
				e_fatal("value expected for option '%s'\n", argv[i]);
				return 1;
			}

			valexpected = 0;
		}

        // Check if there are undefined values at the end of parsing options
        if (valexpected != 0) {
			e_fatal("value expected for option '%s'\n", "test valexpected");
            return 1;
        }

		// Check for values that shouldn't be there
		
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

int valExpected(char opt) {
    int i = 0;
    while(!(g_optslist[i] == 0)) {
        if (g_optslist[i] == opt) {
			// Multiple values
            if (g_optslist[i + 1] == ':') {
                return 2;
            }
			// Single value
			if (g_optslist[i + 1] == '.') {
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

int execute(int (*command)(char **args), char **argv, char **args, int argscount, char *accepted) {
	if (parseOptions(argv + argscount) == 0) {
		if ((unacceptedOptions(accepted)) == -1) {
			command(args);
			return 0;
		} else {
			e_fatal("unknown option '%c'\n", g_optslist[unacceptedOptions(accepted)]);
			help(args);
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

void version(void) {
	char *operatingsystem = "unknown";
	char *architecture = "unknown";

	// Get current architecture
	#if defined(__x86_64__) || defined(_M_X64)
        architecture = "x86_64";
    #elif defined(__i386__) || defined(_M_IX86)
        architecture = "x86";
    #elif defined(__arm__) || defined(_M_ARM)
 		architecture = "ARM";
    #elif defined(__aarch64__)
        architecture = "ARM64";
    #elif defined(__powerpc64__)
        architecture = "PowerPC64";
    #elif defined(__mips__)
        architecture = "MIPS";
    #else
        architecture = "unknown";
    #endif

	// Get current operating system
	#if defined(_WIN32) || defined(_WIN64)
		operatingsystem = "Windows";
    #elif defined(__linux__)
		operatingsystem = "Linux";
    #elif defined(__APPLE__) && defined(__MACH__)
		operatingsystem = "Apple";
    #elif defined(__FreeBSD__)
		operatingsystem = "FreeBSD";
    #elif defined(__unix__) || defined(__unix)
		operatingsystem = "unix";
    #else
		operatingsystem = "unknown";
    #endif	
	
	// Don't support Windows
	if (strcmp(operatingsystem, "Windows") == 0) {
		printf("adess version %s (%s %s)\n", VERSION, operatingsystem, architecture);
		printf("\033[1m" "warning" "\033[m" ": adess may not be fully supported\n");
		return;
	}

	printf("adess version %s (%s %s)\n", VERSION, operatingsystem, architecture);
}
