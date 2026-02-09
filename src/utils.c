#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>

#define DCOL "\x1b[1;35m" // Debug color
#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[m"

#include "utils.h"

void e_fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, COLOR_BOLD "fatal: " COLOR_RESET "");
    vprintf(format, args);
    va_end(args);
}

void e_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, COLOR_BOLD "warning: " COLOR_RESET "");
    vprintf(format, args);
    va_end(args);
}

void e_parse(char *path, int linenr, const char *format, ...) {
	va_list args;
	va_start(args, format);

	// If 'linenr' is 0, don't print the line number
	if (linenr == 0) {
		fprintf(stderr, COLOR_BOLD "fatal: " COLOR_RESET "[error while parsing %s] ", path);
	} else {
		fprintf(stderr, COLOR_BOLD "fatal: " COLOR_RESET "[error while parsing %s:%i] ", path, linenr);
	}

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

void endPrint(uint16_t length, const char *format, ...) {
    va_list args;
    va_start(args, format);

    uint16_t i = 0;
    uint16_t width = getWidth() - length;

    printf("\r");

    while (i < width) {
        printf(" ");
        i++;
    }

    vprintf(format, args);
    va_end(args);
}

uint16_t getWidth(void) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w); // WTF?!
    return w.ws_col;
}

char *onlyFile(char *input) {
    char *cursor = input;
    char *output = cursor;

    while (cursor[0] != '\0') {
        if (cursor[0] == '/') {
            output = cursor;
        }
        cursor++;
    }

    output++;

    return output;
}

/*
void d_print(const char *format, ...) {
	if (g_debug) {
    	va_list args;
    	va_start(args, format);
    	printf(DCOL "[DEBUG] " COLOR_RESET "");
    	vprintf(format, args);
    	va_end(args);
	}
}
*/
