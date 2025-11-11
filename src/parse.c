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

extern char g_debug;
extern char *g_options;

int parseCommand(int argc, char **argv) {
	int argscount = countArgs(argc, argv) - 1;

	bool options[MAX_OPT_COUNT] = {false};

	// Check if first argument is a real argument or option
	char *arg = NULL;
	if (argv[2]) {
		if (!(argv[2][0] == '-')) {
			arg = argv[2];
		}
	}
	
	// Arguments for the execute function int argc, char **argv, bool *options, int argscount, char *accepted, char *arg

	// help
	if (strcmp(argv[1], "help") == 0) {
		return execute(help, argc, argv, options, argscount, "", arg);
	}
	// make (mk)
	else if (((strcmp(argv[1], "make")) == 0) || ((strcmp(argv[1], "mk")) == 0)) {
		return execute(make, argc, argv, options, argscount, "", arg);
	}
	// edit (ed)
	else if (((strcmp(argv[1], "edit")) == 0) || ((strcmp(argv[1], "ed")) == 0)) {
		printf("Edit command\n");
		parseOptions(options, argc - argscount, argv + argscount);
		return 0;
	}
	// view (vw)	
	else if (((strcmp(argv[1], "view")) == 0) || ((strcmp(argv[1], "vw")) == 0)) {
		printf("View command\n");
		parseOptions(options, argc - argscount, argv + argscount);
		return 0;
	}
	// remove (rm)
	else if (((strcmp(argv[1], "remove")) == 0) || ((strcmp(argv[1], "rm")) == 0)) {
		printf("Remove command\n");
		parseOptions(options, argc - argscount, argv + argscount);
		return 0;
	}
	// default (unknown command)
	else {
		printf("adess: '%s' is not an adess command. See 'adess --help'\n", argv[1]);
		return 1;
	}
	return 1;
}

int parseOptions(bool *options, int argc, char **argv) {
	int opt;

	while ((opt = getopt(argc, argv, ":if:lr")) != -1) {
		switch (opt) {
			case 'i':
				options[0] = true;
				break;
			case 'l':
				options[1] = true;
				break;
			case 'r':
				options[2] = true;
				break;
			case 'f':
				options[3] = true;
				printf("filename: %s\n", optarg);
				break;
			case ':':
				d_printFail("no value provided (parseOptions)");
				printf("fatal: no value specified for option '%c'\n", optopt);
				return 1;
			case '?':
				d_printFail("unknown option (parseOptions)");
				printf("fatal: unknown option '%c' (this option is never used)\n", optopt);
				return 1;
		}
	}
	return 0;
}

int execute(int (*command)(char *arg, bool *opts), int argc, char **argv, bool *options, int argscount, char *accepted, char *arg) {
	if (parseOptions(options, argc - argscount, argv + argscount) == 0) {
		if ((unacceptedOptions(options, accepted)) == -1) {
			command(arg, NULL);
			return 0;
		} else {
			printf("fatal: unknown option '%c'\n", g_options[unacceptedOptions(options, accepted)]);
			help(argv[1], NULL);
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
	while (true) {
		if (g_options[i] == 0) {
			break;
		}
		if (options[i] == 1) {
			int n = 0;
			while (true) {
				// If iterated through entire accepted array and didn't find it
				if (accepted[n] == 0) {
					return i;
				}
				// If option is accepted
				if (g_options[i] == accepted[n]) {
					break;
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
