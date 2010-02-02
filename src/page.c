#include "common.h"

#include "page.h"
#include "stdio.h"
#include "register.h"
#include "mem.h"
#include "string.h"
#include "irq.h"

#include "emulator.h"

u8int page_enabled=0;

u32int *page_directory=0;
u32int *page_tables=0;

extern u32int heap;
extern u32int _brk_end;

extern u32int read_cr0();
extern void write_cr0(u32int cr0);
extern u32int read_cr2();
extern u32int read_cr3();
extern void write_cr3(u32int cr3);

#define PANIC(msg) panic(msg, __FILE__, __LINE__);

static inline void page_fault(struct registers regs)
{
    printf_bochs("page_fault: %x\n", read_cr2());
    PANIC("page fault\n");
}

void set_pte_v2p(u32int vaddr, u32int pfn)
{
    u32int pd_index, pt_index;
    u32int pde;
    u32int pte;
    u32int *page_table;

    printf_bochs("create vaddr:%x to pfn: %x\n", vaddr, pfn);

    pd_index = vaddr / 0x400000;
    pt_index = vaddr / 0x1000 % 1024;

    printf_bochs("pd_index:%x pt_index:%x\n", pd_index, pt_index);

    pde = page_directory[pd_index];
    if(!pde) {
        u32int page_table_pfn;

        page_table_pfn = alloc_page_pfn();;
        // ocassionally frame for page table has not been mapped.
        set_pte_v2p(page_table_pfn<<12, page_table_pfn);

        pde = page_table_pfn << 12 | 0x23;
        page_directory[pd_index] = pde;

        page_table = (u32int*)(page_table_pfn<<12);
    } else {
        page_table = (u32int*)(pde & 0xFFFFF000);
    }

    pte = pfn << 12 | 0x23;
    page_table[pt_index] = pte;

    write_cr3(read_cr3());
}

int init_paging()
{
    u32int addr;

    page_directory = (u32int *)alloc_bootmem(0x1000, 1);
    memset(page_directory, 0, 0x1000);

    printf_bochs("page_directory at %x\n", page_directory);

/*
struct page_directory_entry {
    u8int present:1;
    u8int read_or_write:1;
    u8int level:1;
    u8int reserved1:2;
    u8int accessed:1;
    u8int dirty:1;
    u8int reserved2:2;
    u8int avail:3;
    u32int page_frame_address:20;
} __attribute__((packed));
*/
    printf_bochs("_brk_end: %x\n", _brk_end);
    for (addr = 0; addr < _brk_end; addr += 0x1000) {
        u32int pd_index;
        u32int pt_index;
        page_directory_entry_t pde;
        page_table_entry_t pte;
        u32int *page_table;

        pd_index = addr / 0x400000; // addr/4M
        pt_index = addr / 0x1000 % 1024;

        //printf_bochs("pd_index:%x page_directory[pd_index]=%x, pt_index:%x\n", pd_index, page_directory[pd_index], pt_index);

        if (!page_directory[pd_index]) {
            pde.present = 1;
            pde.read_or_write = 1;
            pde.level = 0;
            pde.reserved1 = 0;
            pde.accessed = 1;
            pde.dirty = 0;
            pde.reserved2 = 0;
            pde.avail = 0;

            page_table = (u32int *)alloc_bootmem(0x1000,1);
            pde.page_frame_address = (long)page_table / 0x1000; // addr/4k
            page_directory[pd_index] = pde.page_frame_address << 12 | 0x23 ;

            //printf_bochs("page_table at %x\n", page_table);
            //printf_bochs("page_directory[%x]@%x=%x, %x ~ %x\n", pd_index, &(page_directory[pd_index]), page_directory[pd_index], addr, addr + 0x400000);
        }

        if (!page_table[pt_index]) {
            pte.present = 1;
            pte.read_or_write = 1;
            pte.level = 0;
            pte.reserved1 = 0;
            pte.accessed = 1;
            pte.dirty = 0;
            pte.reserved2 = 0;
            pte.avail = 0;

            pte.page_frame_address = addr / 0x1000; // addr/4k
            page_table[pt_index] = pte.page_frame_address << 12 | 0x23 ;

            //printf_bochs( "page_table[%x]=%x, %x ~ %x\n", pt_index, page_table[pt_index], addr, addr+0x1000);
        }
    }

    write_cr3((u32int)page_directory);
    write_cr0(read_cr0() | 0x80000000);
    page_enabled=1;
    //BOCHS_DEBUGGER_ENTER;

    register_irq_service(14, page_fault);

    return 0;
}


