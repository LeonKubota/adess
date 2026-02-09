#ifndef UTILS_H
#define UTILS_H

#include "main.h"

void e_fatal(const char *format, ...);
void e_warning(const char *format, ...);
void e_parse(char *path, int linenr, const char *format, ...);
void n_print(const char *format, ...);

// For pretty printing
void endPrint(uint16_t length, const char *format, ...);

uint16_t getWidth(void);
char *onlyFile(char *input);

//void d_print(const char *format, ...);

#endif
