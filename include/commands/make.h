#include <stdbool.h>

#ifndef MAKE_H
#define MAKE_H 

#include "main.h"
#include "files/write.h"
#include "files/check.h"

int make(char *arg);

int project();

char *getCurDirectory(char *relativedirectory);
int makeDirectory(char *directory);
int makeFile(char *directory);

#endif
