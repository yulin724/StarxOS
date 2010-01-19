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
static struct desc_struct idt_entries[IDT_ENTRY_MAX];
extern long idt_handlers[];

void initialize_gdt()
{
	struct desc_ptr desc_ptr;
	
	gdt_entries[GDT_ENTRY_KERNEL_CS] = FULL_EXEC_SEGMENT;
	gdt_entries[GDT_ENTRY_KERNEL_DS] = FULL_SEGMENT;
	
	desc_ptr.size = sizeof(gdt_entries);
	desc_ptr.address = (long)(&gdt_entries);
	
	load_gdt(&desc_ptr);
}

void initialize_idt()
{
	int i=0;
	struct desc_ptr desc_ptr;
	
	initialize_8259a();
	
	for(i=0; i < 2; i++) {
		//printf("%x\n", idt_handlers[i] );
		idt_entries[i].a = ( 0x8 ) << 16 | (idt_handlers[i] & 0xFFFF);
		idt_entries[i].b = ( idt_handlers[i] & 0xFFFF0000 ) | 0x8E00;
	}
	
	desc_ptr.size = sizeof(idt_entries);
	desc_ptr.address = &idt_entries;
	
	load_idt(&desc_ptr);
}

void do_IRQ(int eax)
{
}
