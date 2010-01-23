#include "common.h"

#include "multiboot.h"
#include "irq.h"
#include "mem.h"

#include "port.h"
#include "emulator.h"

#include "test.h"

extern long code;
extern long data;
extern long bss;
extern long end;

u32int total_memory, mem_upper, mem_lower;

void do_test()
{
    test_mem();
}

int start_kernel(unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;

    int i=0;
    int x=0,y=0;
    void *ptr1, *ptr2, *ptr3, *ptr4;

    initialize_gdt();

    screen_init_early();
    screen_reset(0);
    printf("Welcome to StarxOS!\n");

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: %x\n", (unsigned) magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf_bochs("Welcome to StarxOS!\n");
    printf_bochs("flags = 0x%x\n", (unsigned) mbi->flags);

    /* Are mem_* valid? */
    if ( mbi->flags != 0)
        printf_bochs ("mem_lower = %x Byte, mem_upper = %x Byte\n",
                 (unsigned) mbi->mem_lower * 1024, (unsigned) mbi->mem_upper * 1024);

    printf_bochs("kernel code: %p\n", &code);
    printf_bochs("kernel data: %p\n", &data);
    printf_bochs("kernel bss : %p\n", &bss);
    printf_bochs("kernel end : %p\n", &end);

    initialize_idt();

    init_timer(50);

    asm volatile("sti");

    mem_lower = mbi->mem_lower * 0x400;
    mem_upper = mbi->mem_upper * 0x400;

    printf_bochs("init_mem\n");
    init_memblk((long)&end, ((long)&end) + 0x400000);
    print_memblks_addr_list();
    printf_bochs("init_mem complete\n\n");


    //do_test();

    init_page(mem_lower, mem_upper);

    bochs_shutdown();

    for(;;);
    return 0xBAD;
}
