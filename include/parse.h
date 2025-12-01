#include <stdbool.h>

#ifndef PARSE_H 
#define PARSE_H 

#include "main.h"

int parse(int argc, char **argv);

int parseCommand(int argc, char **argv);
int parseOptions(char **argv);
bool simpleIsValid(char opt);
int valExpected(char opt);
int optIndex(char opt);

int execute(int (*command)(char **args), int argc, char **argv, int argscount, char *accepted);

int unacceptedOptions(char *accepted);
int countArgs(int argc, char **argv);
void version(char *version);

#endif
