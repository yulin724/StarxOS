#ifndef STDIO_H
#define STDIO_H

#include "config.h"

#define STDOUT (void *)0
#define BOCHS_CONSOLE (void *)1

#ifdef VERBOSE
#define verbose(args...) do { sprintf(STDOUT, args, ""); } while(0);
#else
#define verbose(args...)
#endif

#define printf(args...) do { sprintf(STDOUT, args, ""); } while(0);
#define printf_bochs(args...) do { sprintf(BOCHS_CONSOLE, args, ""); } while(0);

int sprintf(char *str, const char *format, ...);

#endif /* STDIO_H */
