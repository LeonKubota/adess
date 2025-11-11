#include <stdbool.h>

#ifndef PARSE_H 
#define PARSE_H 

int parseCommand(int argc, char **argv);
int parseOptions(bool *options, int argc, char **argv);
int execute(int (*command)(char *arg, bool *opts), int argc, char **argv, bool *options, int argscount, char *accepted, char *arg);
int unacceptedOptions(bool *options, char *accepted);
int countArgs(int argc, char **argv);
void d_printFail(char *name);

#endif
