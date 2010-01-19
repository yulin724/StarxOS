#ifndef STDIO_H
#define STDIO_H

#include "stdarg.h"

#define STDOUT (void *)0

#define printf(args...) sprintf(STDOUT, args, "");

int sprintf(char *str, const char *format, ...);

#endif /* STDIO_H */
