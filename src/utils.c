#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define DCOL "\x1b[1;35m" // Debug color
#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[m"

#include "utils.h"

void e_fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf(COLOR_BOLD "fatal: " COLOR_RESET "");
    vprintf(format, args);
    va_end(args);
}

void e_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf(COLOR_BOLD "warning: " COLOR_RESET "");
    vprintf(format, args);
    va_end(args);
}

void e_parse(char *path, int linenr, const char *format, ...) {
	va_list args;
	va_start(args, format);

	// If 'linenr' is 0, don't print the line number
	if (linenr == 0) {
		printf(COLOR_BOLD "fatal: " COLOR_RESET "[error while parsing %s] ", path);
	} else {
		printf(COLOR_BOLD "fatal: " COLOR_RESET "[error while parsing %s:%i] ", path, linenr);
	}

	vprintf(format, args);
	va_end(args);
}

void b_todo(const char *format, ...) {
	va_list args;
	va_start(args, format);
	printf(COLOR_BOLD "\033[91;103m" "[TODO]" "\t");
	vprintf(format, args);
	printf(COLOR_RESET);
	va_end(args);
}

void n_print(const char *format, ...) {
	va_list args;
	va_start(args, format);
	printf("adess: ");
	vprintf(format, args);
	va_end(args);
}

void d_print(const char *format, ...) {
	if (g_debug) {
    	va_list args;
    	va_start(args, format);
    	printf(DCOL "[DEBUG] " COLOR_RESET "");
    	vprintf(format, args);
    	va_end(args);
	}
}
