#ifndef PAGE_H
#define PAGE_H

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

typedef struct page_directory_entry page_directory_entry_t;
typedef struct page_directory_entry page_table_entry_t;

int init_paging();

void set_pte_v2p(u32int vaddr, u32int pfn);

#endif
