#ifndef DESC_DEFS_H
#define DESC_DEFS_H

struct desc_struct {
	union {
		struct {
			unsigned int a;
			unsigned int b;
		};
		struct {
			u16int limit0;
			u16int base0;
			unsigned base1: 8, type: 4, s: 1, dpl: 2, p: 1;
			unsigned limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
		};
	};
} __attribute__((packed));

struct desc_ptr {
	unsigned short size;
	unsigned long address;
} __attribute__((packed));

#endif
