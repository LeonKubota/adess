#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define COLOR_BOLD "\e[1m"
#define COLOR_RESET "\e[m"

void e_fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf(COLOR_BOLD "fatal: " COLOR_RESET "");
    vprintf(format, args);
    va_end(args);
}

void n_out(const char *format, ...) {
	va_list args;
	va_start(args, format);
	printf("adess: ");
	vprintf(format, args);
	va_end(args);
}
