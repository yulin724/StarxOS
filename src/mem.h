#ifndef MEM_H
#define MEM_H

#include "common.h"

void *pmalloc(u32int size, int align);
void pfree(void *ptr);

void setup_memory(u32int mem_lower, u32int mem_upper);
void *alloc_bootmem(u32int size, int align);

u32int alloc_page_pfn();

#endif
