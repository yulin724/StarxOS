#include "common.h"

struct mcb {
    struct list_head memblk_size_list_node;
    struct list_head memblk_addr_list_node;
    u32int size;
    u8int available;
    u32int start_addr;
};

struct list_head memblks_size;
struct list_head memblks_addr;

struct mcb *init_one_memblk(u32int start_addr, u32int size) {
    struct mcb *blk;

    blk = (struct mcb *)( start_addr + size - sizeof(struct mcb) );
    blk->size = size - sizeof(struct mcb);
    blk->start_addr = start_addr;

    return blk;
}

void print_memblks_addr_list()
{
    struct list_head *p;
    struct mcb *blk;

    printf_bochs("memblks_addr_list\n");
    printf_bochs("-------------------------------------------------\n");
    list_for_each(p, &memblks_addr) {
            blk = list_entry(p, struct mcb, memblk_addr_list_node);
        printf_bochs("mcb@%p, mem@%x, size:%x, available:%x\n",
            blk, blk->start_addr, blk->size, blk->available);
    }
    printf_bochs("=================================================\n");
}

void print_memblks_size_list()
{
    struct list_head *p;
    struct mcb *blk;

    printf_bochs("memblks_size_list\n");
    printf_bochs("-------------------------------------------------\n");
    list_for_each(p, &memblks_size) {
            blk = list_entry(p, struct mcb, memblk_size_list_node);
        printf_bochs("mcb@%p, mem@%x, size:%x, available:%x\n",
            blk, blk->start_addr, blk->size, blk->available);
    }
    printf_bochs("=================================================\n");
}

void init_memblk(u32int mem_lower, u32int mem_upper)
{
    struct mcb *blk;

    INIT_LIST_HEAD( &memblks_size );
    INIT_LIST_HEAD( &memblks_addr );

    verbose("init_mem\n");
    blk = init_one_memblk(mem_lower, mem_upper - mem_lower);
    blk->available = 1;
    list_add( &(blk->memblk_size_list_node), &memblks_size);
    list_add( &(blk->memblk_addr_list_node), &memblks_addr);
}

void insert_sort_by_size(struct list_head *new,struct list_head *head)
{
    struct list_head *blk_list_head;
    struct mcb *new_blk;
    struct mcb *curr;
    struct list_head *p;

    blk_list_head = new;
    new_blk = list_entry(new, struct mcb, memblk_size_list_node);

    verbose("3. try to insert blk %p\n", new_blk);

    list_for_each(p, &memblks_size) {
        curr = list_entry(p, struct mcb, memblk_size_list_node);

        if ( new_blk->size <= curr->size ) {
            p->prev->next = new;
            new->prev = p->prev;
            new->next = p;
            p->prev = new;
            return;
        }

    }
    list_add_tail(new, &memblks_size);
}

struct mcb* split_from(struct mcb *orig_blk, u32int size, int align) {
    struct mcb *blk;

    if(align)
        orig_blk->start_addr = ( (long)orig_blk - size ) & 0xFFFF1000;
    else
        orig_blk->start_addr = ( (long)orig_blk - size );
    printf_bochs("orig_blk->start_addr :%p\n", orig_blk->start_addr);
    size = (long)orig_blk - orig_blk->start_addr;
    if ( orig_blk->size > size + sizeof(struct mcb) ) {
        blk = (struct mcb *)( (long)orig_blk - size - sizeof(struct mcb) );

        blk->size = orig_blk->size - size - sizeof(struct mcb);
        blk->available = 1;
        blk->start_addr = (long)blk - blk->size;

        // we use orig_blk as the new allocated mcb
        orig_blk->available = 0;
        orig_blk->size = size;

        verbose("2. cut %x from %p. new blk:%p, size:%x, available:%x\n",
                size, orig_blk, blk, blk->size, blk->available);

        if ( (&memblks_size)->next->next == (&memblks_size) ) {
            verbose("only 1 memblk in list\n");
            list_add_tail( &(blk->memblk_size_list_node), &memblks_size );
            list_add( &(blk->memblk_addr_list_node), &memblks_addr );
            return blk;
        }

        list_del( &(orig_blk->memblk_size_list_node) );
        insert_sort_by_size( &(orig_blk->memblk_size_list_node), &memblks_size );
        insert_sort_by_size( &(blk->memblk_size_list_node), &memblks_size );

        {   // add new element, sorted by address
            struct list_head *prev, *new, *curr;

            new = &(orig_blk->memblk_addr_list_node);
            curr  = &(blk->memblk_addr_list_node);
            prev = curr->prev;

            prev->next = new;
            new->prev = prev;

            new->next = curr;
            curr->prev = new;
        }
    }
    return blk;
}

static void try_merge(struct list_head *p)
{
    struct mcb *curr_memblk, *prev_memblk;

    curr_memblk = list_entry(p, struct mcb, memblk_addr_list_node);
    prev_memblk = list_entry(p->prev, struct mcb, memblk_addr_list_node);

    printf_bochs("trying to merge %p\n", curr_memblk);
    printf_bochs( "%x + %x == %x ??? \n", (long)(prev_memblk+1), curr_memblk->size, (long)curr_memblk );
    if ( curr_memblk->available && prev_memblk->available
         && (long)(prev_memblk+1) + curr_memblk->size == (long)curr_memblk )
    {
        curr_memblk->size += sizeof(struct mcb) + prev_memblk->size;
        printf_bochs( "merge %p into %p, newsize:%x\n", curr_memblk, prev_memblk, curr_memblk->size );
        p->prev->prev->next = p;
        p->prev = p->prev->prev;
        curr_memblk->start_addr = prev_memblk->start_addr;

        {   // change element in memblk_size list
            struct list_head *p1, *p2, *newp;
            p1 = &(curr_memblk->memblk_size_list_node);
            p2 = &(prev_memblk->memblk_size_list_node);
            newp = &(curr_memblk->memblk_size_list_node);
            list_del(p1);
            list_del(p2);
            insert_sort_by_size(newp, &memblks_size);
        }
    }
}

void pfree(void *ptr)
{
    struct list_head *p;
    struct mcb *blk;

    printf_bochs("try to free %p\n", ptr);
    list_for_each(p, &memblks_addr)
    {
        blk = list_entry(p, struct mcb, memblk_addr_list_node);
        if ( blk->start_addr == (long)ptr )
            break;
    }
    blk->available = 1;

    verbose("free %p, blk at:%p\n", ptr, blk);

    try_merge( &(blk->memblk_addr_list_node) );
}

static struct mcb* find_first_proper_memblk(u32int size, int align) {
    struct list_head *p;
    struct mcb *new_blk;

    list_for_each(p, &memblks_size) {
        new_blk = list_entry(p, struct mcb, memblk_size_list_node);
        if ( new_blk->available && new_blk->size >= sizeof(struct mcb) + size ) {
            if(align) {
                u32int start;
                start = ( (long)new_blk - size ) & 0xFFFF1000;
                printf_bochs("check mcb@%p origstart:%x, start:%x\n", new_blk, new_blk->start_addr , start);
                if (start >= new_blk->start_addr)
                    return new_blk;
            } else
                return new_blk;
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
        printf_bochs("orig_mcb@%p, new_mcb@%p, mem start @ %x\n", new_blk, (long)new_blk - size);
        return (void*) (orig_blk->start_addr);
    } else {
        printf_bochs("no proper blk\n");
        return 0;
    }
}
