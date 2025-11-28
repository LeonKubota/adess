#include <stdbool.h>

#ifndef MAKE_H
#define MAKE_H 

#include "main.h"
#include "files/write.h"
#include "files/check.h"

int make(char *arg);

int project();
int engine();

void appendProjectDefault(char *path);
int makeDirectory(char *path);
int makeFile(char *path);

#endif
