#include "kheap.h"

#include "emulator.h"

#define KHEAP_BASE (0xC0000000+0x400000)
#define KHEAP_TOP (0xC0000000+0x400000*0x10)

u32int kheap_base;
u32int kheap_cur;

void init_kheap()
{
    kheap_base = kheap_cur = KHEAP_BASE;

}

void *kmalloc(u32int size)
{
    u32int ret;
    u32int pfn;

    /* 这里应该要引入heap的管理机制，管理的是连续的虚拟地址 */
    ret = kheap_cur;
    kheap_cur += size;

    if(kheap_cur > KHEAP_TOP)
        return (void *)0;

    pfn = alloc_page_pfn();
    set_pte_v2p(ret, pfn);

    return (void*)ret;
}

void *kmalloc_align(u32int size)
{
    void *ret;

    kheap_cur = (kheap_cur + 0x1000) % 0xFFFFF000;
    ret = (void*)kheap_cur;

    return kmalloc(size);
}

void kfree(const void *ptr)
{
    printf_bochs("kfree not implemented.\n");
}


