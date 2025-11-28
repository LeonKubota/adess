#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

int appendLine(char *path, const char *format, ...) {
	va_list args;
	va_start(args, format);

	FILE *file;
	
	file = fopen(path, "a"); // 'a' is for append

	if (file == NULL) {
		fclose(file);
		e_fatal("failed to write into file '%s'\n", path);
	}

	vfprintf(file, format, args);

	va_end(args);
	fclose(file);
	return 0;
}
