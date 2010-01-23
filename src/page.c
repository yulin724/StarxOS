#include "common.h"

#include "page.h"
#include "stdio.h"

u8int page_enabled=0;

u32int *page_directory=0;
u32int *page_tables=0;

extern u32int heap;

int init_page(u32int lower, u32int upper)
{
    u32int page_nr, page_table_nr;
    u32int total_size_of_page_tables;

    u32int addr;

    page_nr = upper / 0x1000 + 1;
    page_table_nr = page_nr / 1024 + 1;

    // page directory(4k) + page_table_nr * 4k
    total_size_of_page_tables = 0x1000 + page_table_nr * 0x1000;

    page_directory = (u32int *)pmalloc(total_size_of_page_tables, 1);
    memset(page_directory, 0, total_size_of_page_tables);
    page_tables = (u32int *)page_directory + 0x1000;
    printf_bochs("page_directory at %x, page_table at %x\n",
        page_directory, page_tables);

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
    for(addr = 0; addr < heap+0x400000*2 ; addr += 0x1000) {
        u32int pd_index;
        u32int pt_index;
        page_directory_entry_t pde;
        page_table_entry_t pte;
        u32int *page_table;

        pd_index = addr / 0x400000; // addr/4M
        pt_index = addr / 0x1000 % 1024;

        page_table = (u32int *) ( (long)page_tables + 0x1000 * pd_index );

        if(!page_directory[pd_index]) {
            pde.present = 1;
            pde.read_or_write = 1;
            pde.level = 0;
            pde.reserved1 = 0;
            pde.accessed = 1;
            pde.dirty = 0;
            pde.reserved2 = 0;
            pde.avail = 0;
            pde.page_frame_address = (long)page_table / 0x1000; // addr/4k
            page_directory[pd_index] = pde.page_frame_address << 20 | 0x23 ;
            printf_bochs("page_table at %x\n", page_table);
            printf_bochs("page_directory[%x]=%x\n", pd_index, page_directory[pd_index]);
        }

        if(!page_table[pt_index]) {
            pte.present = 1;
            pte.read_or_write = 1;
            pte.level = 0;
            pte.reserved1 = 0;
            pte.accessed = 1;
            pte.dirty = 0;
            pte.reserved2 = 0;
            pte.avail = 0;
            pte.page_frame_address = addr / 0x1000; // addr/4k
            page_tables[pt_index] = pte.page_frame_address << 20 | 0x23 ;
            printf_bochs( "page_tables[%x]=%x\n", pt_index, page_tables[pt_index] );
        }

    }
    return 0;
}
