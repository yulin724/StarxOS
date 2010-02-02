#ifndef STRING_H
#define STRING_H

#include "type_alias.h"

void *memset(void *s, u8int c, u32int count);

void *memcpy(void *dest, const void *src, int count);

int strlen(const char *s);


#endif /* STRING_H */
