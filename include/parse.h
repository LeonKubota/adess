#include <stdbool.h>

#ifndef PARSE_H 
#define PARSE_H 

#include "struct_value.h"

int parseCommand(int argc, char **argv);
int parseOptions(bool *options, Values *values, char **argv);
bool simpleIsValid(char opt);
bool valExpected(char opt);
int optIndex(char opt);
int execute(int (*command)(char *arg, bool *opts, Values *vals), int argc, char **argv, bool *options, Values *values, int argscount, char *accepted, char *arg);
int unacceptedOptions(bool *options, char *accepted);
int countArgs(int argc, char **argv);
void d_printFail(char *name);

#endif
