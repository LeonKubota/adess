#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "commands/command.h"
#include "commands/make.h"

int make(char *arg, bool *opts) {
	d_showInput("make", arg, opts);
	
	printf("MAKE RAN");
	return 0;
}
