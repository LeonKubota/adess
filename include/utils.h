#ifndef UTILS_H
#define UTILS_H

#include "main.h"

void e_fatal(const char *format, ...);
void e_warning(const char *format, ...);
void e_parse(char *path, int linenr, const char *format, ...);
void b_todo(const char *format, ...);
void n_print(const char *format, ...);
void d_print(const char *format, ...);

#endif
