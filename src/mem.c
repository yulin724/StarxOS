#include "common.h"

struct mcb {
    struct list_head memblk_size_list_node;
    struct list_head memblk_addr_list_node;
    u32int start_addr;
    u32int available;
};

struct list_head memblks_by_size;
struct list_head memblks_by_addr;


extern u32int _brk_base;
u32int _brk_start;
u32int _brk_end;

u32int *frame_bitmap;
u32int frame_nr;


void print_memblks_addr_list()
{
    struct list_head *p;
    struct mcb *blk;
    u32int prev_addr=0;

    printf_bochs("memblks_by_addr list\n");
    printf_bochs("-------------------------------------------------\n");
    list_for_each(p, &memblks_by_addr)
    {
        blk = list_entry(p, struct mcb, memblk_addr_list_node);
        printf_bochs("mcb@%p, start@%x, avail:%x, size:%x\n",
            blk, blk->start_addr, blk->available, (long)blk - blk->start_addr);
        if ( (long)blk < prev_addr ) {
            printf_bochs("!!!!!!! %s %s() %x: memblks_addr_list has error!!!\n",
                __FILE__, __FUNCTION__, __LINE__);
            panic();
        }
        prev_addr = (long)blk;

    }
    printf_bochs("=================================================\n");
}

void print_memblks_size_list()
{
    struct list_head *p;
    struct mcb *blk;
    u32int prev_size=0;

    printf_bochs("memblks_by_size list\n");
    printf_bochs("-------------------------------------------------\n");
    list_for_each(p, &memblks_by_size)
    {
        blk = list_entry(p, struct mcb, memblk_size_list_node);
        printf_bochs("mcb@%p, start@%x, avail:%x, size:%x\n",
            blk, blk->start_addr, blk->available, (long)blk - blk->start_addr);
        if ( (long)blk - blk->start_addr < prev_size ) {
            printf_bochs("!!!!!!! %s %s() %x: memblks_size_list has error!!!\n",
                __FILE__, __FUNCTION__, __LINE__);
            panic();
        }
        prev_size = (long)blk - blk->start_addr;

    }
    printf_bochs("=================================================\n");
}

void init_memblk(u32int mem_lower, u32int mem_upper)
{
    struct mcb *first_memblk;
    u32int whole_size;

    INIT_LIST_HEAD( &memblks_by_size );
    INIT_LIST_HEAD( &memblks_by_addr );

    verbose("init_mem\n");

    whole_size = mem_upper - mem_lower;

    first_memblk = (struct mcb *)( mem_lower + whole_size - sizeof(struct mcb) );
    memset(first_memblk, 0, sizeof(struct mcb));
    first_memblk->start_addr = mem_lower;
    first_memblk->available = 1;

    list_add( &(first_memblk->memblk_size_list_node), &memblks_by_size);
    list_add( &(first_memblk->memblk_addr_list_node), &memblks_by_addr);
}

void insert_sort_by_size(struct list_head *to_insert,struct list_head *head)
{
    struct list_head *p;
    struct mcb *to_insert_mcb, *p_mcb;
    u32int to_insert_size, p_size;

    to_insert_mcb = list_entry(to_insert, struct mcb, memblk_size_list_node);
    to_insert_size = (long)to_insert_mcb - to_insert_mcb->start_addr;

    printf_bochs("trying to insert mcb@%p into memblks_by_size \n", to_insert_mcb);

    list_for_each(p, &memblks_by_size) {
        p_mcb = list_entry(p, struct mcb, memblk_size_list_node);
        p_size = (long)p_mcb - p_mcb->start_addr;
        if ( to_insert_size <= p_size) {
            // we found some element's size bigger than my own.
            p->prev->next = to_insert;
            to_insert->prev = p->prev;

            to_insert->next = p;
            p->prev = to_insert;
            return;
        }
    }
    // this block's size is bigger than all others.
    list_add_tail(to_insert, &memblks_by_size);
}

struct mcb* split_from(struct mcb *orig_blk, u32int size, int align)
{
    struct mcb *mcb1, *mcb2;
    u32int orig_blk_start_addr;
    u32int orig_blk_available;

    struct list_head *p_mcb1, *p_mcb2;

    orig_blk_start_addr = orig_blk->start_addr;
    orig_blk_available = orig_blk->available;

    // this mcb2 is the return value
    mcb2 = orig_blk;
    // adjust mcb2, start_addr and aligned start_address
    if (align) {
        mcb2->start_addr = ( (long)mcb2 - size ) & 0xFFFFF000;
    } else {
        mcb2->start_addr = ( (long)mcb2 - size );
    }
    mcb2->available = 0;
    size = (long)mcb2 - mcb2->start_addr;

    // create new mcb1, adjust mcb1
    mcb1 = (struct mcb*)( mcb2->start_addr - sizeof(struct mcb) );
    memset(mcb1, 0, sizeof(struct mcb));
    mcb1->start_addr = orig_blk_start_addr;
    mcb1->available = 1;

    printf_bochs("orig@%p, start_addr@%x, avail:%x\n", orig_blk, orig_blk_start_addr, orig_blk_available);
    printf_bochs("mcb1@%p, start_addr@%x, avail:%x, size:%x\n", mcb1, mcb1->start_addr, mcb1->available, (long)mcb1 - mcb1->start_addr);
    printf_bochs("mcb2@%p, start_addr@%x, avail:%x, size:%x\n", mcb2, mcb2->start_addr, mcb2->available, (long)mcb2 - mcb2->start_addr);
    if ( (long)mcb2 + sizeof(struct mcb) - orig_blk_start_addr
        == (long)mcb2 + sizeof(struct mcb) - mcb2->start_addr +  (long)mcb1 + sizeof(struct mcb) - mcb1->start_addr) {
        printf_bochs("space is not missing\n");
    }
    else {
        printf_bochs("!!!!!!!!!!!!!!!!!!! space lost!!!!!!!!!!!!!!!!!!!!!\n");
    }

    /* add mcb1, mcb2 into memblks_by_add, because I split mcb1, mcb2
     * from orig_blk, so mcb1, mcb2 is continus on address
     */
    p_mcb1 = &(mcb1->memblk_addr_list_node);
    p_mcb2 = &(mcb2->memblk_addr_list_node);
    list_add(p_mcb1, &memblks_by_addr);

    // add mcb1, mcb2 into memblks_by_size
    p_mcb1 = &(mcb1->memblk_size_list_node);
    p_mcb2 = &(mcb2->memblk_size_list_node);
    list_del(p_mcb2);

    if ( (&memblks_by_size)->next == (&memblks_by_size) ) {
        printf_bochs("no memblk in memblks_by_size list\n");
        list_add(p_mcb1, &memblks_by_size );
        list_add(p_mcb2, &memblks_by_size );
        return mcb2;
    }

    // if come here, at least there are 2 memblk in memblks_by_size list
    insert_sort_by_size( p_mcb1, &memblks_by_size );
    insert_sort_by_size( p_mcb2, &memblks_by_size );

    return mcb2;
}

static void try_merge(struct list_head *mid)
{
    struct list_head *prev, *next;
    struct mcb *prev_mcb, *mid_mcb, *next_mcb;

    mid_mcb = list_entry(mid, struct mcb, memblk_addr_list_node);
    printf_bochs("-----try merge %p\n", mid_mcb);

    prev = mid->prev;
    next = mid->next;

    if ( mid->prev == &memblks_by_addr ) {
        next_mcb = list_entry(next, struct mcb, memblk_addr_list_node);
        if ( next_mcb->available ) {
            next_mcb->start_addr = mid_mcb->start_addr;
            list_del(mid);
            list_del(&(mid_mcb->memblk_size_list_node));
            list_del(&(next_mcb->memblk_size_list_node));
            insert_sort_by_size(&(next_mcb->memblk_size_list_node), &memblks_by_size);
        }
        return;
    }

    if ( mid->next == &memblks_by_addr ) {
        prev_mcb = list_entry(prev, struct mcb, memblk_addr_list_node);
        if ( prev_mcb->available ) {
            printf_bochs("%s %x\n", __FUNCTION__, __LINE__);
            mid_mcb->start_addr = prev_mcb->start_addr;
            print_memblks_addr_list();
            list_del(prev);
            print_memblks_addr_list();
            list_del(&(prev_mcb->memblk_size_list_node));
            list_del(&(mid_mcb->memblk_size_list_node));
            insert_sort_by_size(&(mid_mcb->memblk_size_list_node), &memblks_by_size);

        }
        return;
    }

    if ( mid->prev != &memblks_by_addr && mid->next != &memblks_by_addr ) {
        prev_mcb = list_entry(prev, struct mcb, memblk_addr_list_node);
        next_mcb = list_entry(next, struct mcb, memblk_addr_list_node);
        if ( prev_mcb->available && next_mcb->available ) {
            next_mcb->start_addr = prev_mcb->start_addr;
            list_del(prev);
            list_del(mid);
            list_del(&(prev_mcb->memblk_size_list_node));
            list_del(&(mid_mcb->memblk_size_list_node));
            list_del(&(next_mcb->memblk_size_list_node));
            insert_sort_by_size(&(next_mcb->memblk_size_list_node), &memblks_by_size);
        }
        if ( !prev_mcb->available && next_mcb->available ) {
            next_mcb->start_addr = mid_mcb->start_addr;
            list_del(mid);
            list_del(&(mid_mcb->memblk_size_list_node));
            list_del(&(next_mcb->memblk_size_list_node));
            insert_sort_by_size(&(next_mcb->memblk_size_list_node), &memblks_by_size);
        }
        if ( prev_mcb->available && !next_mcb->available ) {
            mid_mcb->start_addr = prev_mcb->start_addr;
            list_del(prev);
            list_del(&(prev_mcb->memblk_size_list_node));
            list_del(&(mid_mcb->memblk_size_list_node));
            insert_sort_by_size(&(mid_mcb->memblk_size_list_node), &memblks_by_size);
        }
        if ( !prev_mcb->available && !next_mcb->available ) {
        }
        return;
    }
}

void pfree(void *ptr)
{
    struct list_head *p;
    struct mcb *blk;

    printf_bochs("try to free %p\n", ptr);
    list_for_each(p, &memblks_by_addr)
    {
        blk = list_entry(p, struct mcb, memblk_addr_list_node);
        if ( blk && blk->start_addr == (long)ptr ) {
            blk->available = 1;
            break;
        }
    }

    try_merge( &(blk->memblk_addr_list_node) );
}

static struct mcb* find_first_proper_memblk(u32int size, int align) {
    struct list_head *p;
    struct mcb *blk;

    list_for_each(p, &memblks_by_size) {
        blk = list_entry(p, struct mcb, memblk_size_list_node);
        if ( blk && blk->available
            && ((long)blk - blk->start_addr) >= sizeof(struct mcb) + size ) {
            if(align) {
                u32int start;
                //aligned start
                start = ( (long)blk - size ) & 0xFFFF1000;
                if (start >= blk->start_addr)
                    return blk;
            } else
                return blk;
        }
    }
    return 0;
}

void *pmalloc(u32int size, int align)
{
    struct mcb *orig_blk;
    struct mcb *new_blk;

    if ( size <= 0 )
        return 0;

    orig_blk = find_first_proper_memblk(size, align);

    if (orig_blk) {
        new_blk = split_from(orig_blk, size, align);
        if(new_blk) {
            printf_bochs("return ptr:%p\n", (void*) (new_blk->start_addr) );
            return (void*) (new_blk->start_addr);
        }
        printf_bochs("no proper blk\n");
        return 0;
    }
    printf_bochs("no proper blk\n");
    return (void*)0;
}

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame(u32int frame_addr)
{
   u32int frame = frame_addr/0x1000;
   u32int idx = INDEX_FROM_BIT(frame);
   u32int off = OFFSET_FROM_BIT(frame);
   frame_bitmap[idx] |= (0x1 << off);
}

static void clear_frame(u32int frame_addr)
{
   u32int frame = frame_addr/0x1000;
   u32int idx = INDEX_FROM_BIT(frame);
   u32int off = OFFSET_FROM_BIT(frame);
   frame_bitmap[idx] &= ~(0x1 << off);
}

static u32int test_frame(u32int frame_addr)
{
   u32int frame = frame_addr/0x1000;
   u32int idx = INDEX_FROM_BIT(frame);
   u32int off = OFFSET_FROM_BIT(frame);
   return (frame_bitmap[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static u32int first_frame()
{
   u32int i, j;
   for (i = 0; i < INDEX_FROM_BIT(frame_nr); i++)
   {
       if (frame_bitmap[i] != 0xFFFFFFFF) // nothing free, exit early.
       {
           // at least one bit is free here.
           for (j = 0; j < 32; j++)
           {
               u32int toTest = 0x1 << j;
               if ( !(frame_bitmap[i]&toTest) )
               {
                   return i*4*8+j;
               }
           }
       }
   }

   return -1;
}

void setup_memory(u32int mem_lower, u32int mem_upper)
{
    u32int addr;

    _brk_start = _brk_end = _brk_base;
    frame_bitmap = (u32int *)_brk_end;
    printf_bochs("frame_bitmap @ %x\n", frame_bitmap);

    frame_nr = mem_upper / 0x1000;
    _brk_end = _brk_end + frame_nr / 8 + 1;

    memset(frame_bitmap, 0, _brk_end - (u32int)frame_bitmap);

    for(addr=0; addr < _brk_end; addr+=0x1000) {
        set_frame(addr);
    }
}

void *alloc_bootmem(u32int size, int align)
{
    u32int ret;

    if (align)
        _brk_end = (_brk_end + 0x1000) & 0xFFFFF000;

    ret = _brk_end;
    set_frame(ret);

    _brk_end += size;

    return (void*)ret;
}

u32int alloc_page_pfn()
{
    u32int pfn;

    // find first free frame
    pfn = first_frame();
    set_frame(pfn<<12);

    return pfn;
}
