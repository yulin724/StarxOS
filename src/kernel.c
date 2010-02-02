#include "common.h"

#include "multiboot.h"
#include "irq.h"
#include "mem.h"

#include "port.h"
#include "emulator.h"

#include "gdt_idt.h"
#include "timer.h"
#include "page.h"

#include "kheap.h"

#include "test.h"

extern long code;
extern long data;
extern long bss;
extern long end;

// for global memory range
u32int mem_upper, mem_lower;

// for boot allocator
u32int _brk_base = (u32int)&end;

void do_test()
{
    test_mem();
}

int start_kernel(unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;

    int i=0;

    /* Initialize GDT */
    initialize_gdt();

    screen_init_early();
    screen_reset(0);
    printf("Welcome to StarxOS!\n");

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: %x\n", (unsigned) magic);
        return -1;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf_bochs("Welcome to StarxOS!\n");
    printf_bochs("flags = 0x%x\n", (unsigned) mbi->flags);

    /* Are mem_* valid? */
    if ( mbi->flags != 0)
        printf_bochs ("mem_lower = %x Byte, mem_upper = %x Byte\n",
                 (unsigned) mbi->mem_lower * 0x400, (unsigned) mbi->mem_upper * 0x400);

    printf_bochs("kernel code: %p\n", &code);
    printf_bochs("kernel data: %p\n", &data);
    printf_bochs("kernel bss : %p\n", &bss);
    printf_bochs("kernel end : %p\n", &end);

    /* Initialize IDT */
    initialize_idt();

    /* Initialize Timer */
    init_timer(50);

    /* Enable Interrupt */
    asm volatile("sti");

    mem_lower = mbi->mem_lower * 0x400;
    mem_upper = mbi->mem_upper * 0x400;
    /* Setup memory, and init frame_bitmap */
    setup_memory(mem_lower, mem_upper);

    /* Enable Paging */
    init_paging();

    /* Init heap */
    init_kheap();

    u32int *ptr = (u32int*)kmalloc(0x1000+2);

    *ptr = 0x1234;

    printf_bochs("ptr:%x *ptr:%x\n", ptr, *ptr);
    u32int *ptr2 = (u32int*)kmalloc(0x1000+2);

    *ptr2 = 0x4567;

    printf_bochs("ptr2:%x *ptr2:%x\n", ptr2, *ptr2);

    bochs_enter_debugger();

    bochs_shutdown();

    return 0xBAD;
}
