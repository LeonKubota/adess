#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define DCOL "\x1b[1;35m" // Debug color
#define COLOR_BOLD "\e[1m"
#define COLOR_RESET "\e[m"

#include "utils.h"

void e_fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf(COLOR_BOLD "fatal: " COLOR_RESET "");
    vprintf(format, args);
    va_end(args);
}

void e_parse(char *path, int linenr, const char *format, ...) {
	va_list args;
	va_start(args, format);
	printf(COLOR_BOLD "fatal: " COLOR_RESET "[error while parsing %s:%i] ", path, linenr);
	vprintf(format, args);
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
