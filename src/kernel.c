#include "common.h"

#include "multiboot.h"

extern long code;
extern long data;
extern long bss;
extern long end;

int start_kernel(unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;

    int i=0;
    int x=0,y=0;

	initialize_gdt();
	
    screen_init_early();
    screen_reset(0);
    printf("Welcome to StarxOS!\n");

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf ("Invalid magic number: %x\n", (unsigned) magic);
        return;
    }
    
    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf ("flags = 0x%x\n", (unsigned) mbi->flags);
    
    /* Are mem_* valid? */
    if ( mbi->flags != 0)
		printf ("mem_lower = %x Byte, mem_upper = %x Byte\n",
                 (unsigned) mbi->mem_lower * 1024, (unsigned) mbi->mem_upper * 1024);
	
	printf("kernel code: %x\n", &code);
	printf("kernel data: %x\n", &data);
	printf("kernel bss : %x\n", &bss);
	printf("kernel end : %x\n", &end);
	
	printf("initialize_idt\n");
	initialize_idt();
	
    return 0xBAD;
}
