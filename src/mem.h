#ifndef MEM_H
#define MEM_H

#include "common.h"

void *pmalloc(u32int size, int align);
void pfree(void *ptr);

#endif
