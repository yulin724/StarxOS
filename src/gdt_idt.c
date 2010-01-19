#include "common.h"
#include "desc_defs.h"

#define GDT_ENTRY_MAX 5
#define IDT_ENTRY_MAX 256

#define GDT_ENTRY_KERNEL_CS 0x1
#define GDT_ENTRY_KERNEL_DS 0x2

#define GDT_ENTRY_INIT(flags, base, limit) { { { \
		.a = ((limit) & 0xffff) | (((base) & 0xffff) << 16), \
		.b = (((base) & 0xff0000) >> 16) | (((flags) & 0xf0ff) << 8) | \
			((limit) & 0xf0000) | ((base) & 0xff000000), \
	} } }

#define FULL_EXEC_SEGMENT \
	((struct desc_struct)GDT_ENTRY_INIT(0xc09b, 0, 0xfffff))
#define FULL_SEGMENT ((struct desc_struct)GDT_ENTRY_INIT(0xc093, 0, 0xfffff))


static struct desc_struct gdt_entries[GDT_ENTRY_MAX];
extern struct desc_struct idt_entries;

void initialize_gdt()
{
	struct desc_ptr desc_ptr;
	
	gdt_entries[GDT_ENTRY_KERNEL_CS] = FULL_EXEC_SEGMENT;
	gdt_entries[GDT_ENTRY_KERNEL_DS] = FULL_SEGMENT;
	
	desc_ptr.size = sizeof(gdt_entries);
	desc_ptr.address = (long)(&gdt_entries);
	
	load_gdt(&desc_ptr);
}

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

static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags)
{
	idt_entries[num].base0
}

void initialize_idt()
{
	int i=0;
	
	initialize_8259a();
	
	for(i=0; i < IDT_ENTRY_MAX; i++) {
		
	}
}
