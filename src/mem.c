#include "common.h"

struct memblk_head {
    struct list_head memblk_size_list_node;
    struct list_head memblk_addr_list_node;
    u32int size;
    u8int available;
};

struct list_head memblks_size;
struct list_head memblks_addr;

struct memblk_head *init_one_memblk(u32int start_addr, u32int size) {
    struct memblk_head *blk;

    blk = (struct memblk_head *)start_addr;
    blk->size = size - sizeof(struct memblk_head);

    return blk;
}

void print_memblk_list(u8int type)
{
    struct list_head *p;
    struct memblk_head *blk;

    struct list_head *memblks_list;

#define SORTED_BY_SIZE 0
#define SORTED_BY_ADDR 1

    switch (type) {
    case SORTED_BY_SIZE:
        memblks_list = &memblks_size;
        break;
    case SORTED_BY_ADDR:
        memblks_list = &memblks_addr;
        break;
    default:
        memblks_list = &memblks_addr;
        break;
    }

    list_for_each(p, memblks_list) {
        if ( type == SORTED_BY_SIZE )
            blk = list_entry(p, struct memblk_head, memblk_size_list_node);
        if ( type == SORTED_BY_ADDR )
            blk = list_entry(p, struct memblk_head, memblk_addr_list_node);
        printf_bochs("%x start at:%p, size:%x, available:%x\n", type, blk, blk->size, blk->available);
    }
}

void init_memblk(u32int mem_lower, u32int mem_upper)
{
    struct memblk_head *blk;

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
    struct memblk_head *new_blk;
    struct memblk_head *curr;
    struct list_head *p;

    blk_list_head = new;
    new_blk = list_entry(new, struct memblk_head, memblk_size_list_node);

    verbose("3. try to insert blk %p\n", new_blk);

    list_for_each(p, &memblks_size) {
        curr = list_entry(p, struct memblk_head, memblk_size_list_node);

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

struct memblk_head* split_from(struct memblk_head *orig_blk, u32int size) {
    struct memblk_head *blk;

    if ( orig_blk->size > size + sizeof(struct memblk_head) ) {
        blk = (struct memblk_head *)( ((u32int)(orig_blk+1)) + size);

        blk->size = orig_blk->size - size - sizeof(struct memblk_head);
        blk->available = 1;
        verbose("2. cut %x from %p. new blk:%p, size:%x, available:%x\n",
                size, orig_blk, blk, blk->size, blk->available);

        orig_blk->available = 0;
        orig_blk->size = size;

        if ( (&memblks_size)->next->next == (&memblks_size) ) {
            verbose("only 1 memblk in list\n");
            list_add_tail( &(blk->memblk_size_list_node), &memblks_size );
            list_add_tail( &(blk->memblk_addr_list_node), &memblks_addr );
            return orig_blk;
        }

        list_del( &(orig_blk->memblk_size_list_node) );
        insert_sort_by_size( &(orig_blk->memblk_size_list_node), &memblks_size );
        insert_sort_by_size( &(blk->memblk_size_list_node), &memblks_size );

        {   // add new element, sorted by address
            struct list_head *curr, *new, *next;
            curr = &(orig_blk->memblk_addr_list_node);
            new  = &(blk->memblk_addr_list_node);
            next = curr->next;

            curr->next = new;
            new->prev = curr;
            new->next = next;
            next->prev = new;
        }
    }

    return orig_blk;
}

static void try_merge(struct list_head *p)
{
    struct memblk_head *curr_memblk, *next_memblk;

    curr_memblk = list_entry(p, struct memblk_head, memblk_addr_list_node);
    next_memblk = list_entry(p->next, struct memblk_head, memblk_addr_list_node);

    if ( curr_memblk->available && next_memblk->available
         && (long)(curr_memblk+1) +  curr_memblk->size == (long)next_memblk )
    {
        curr_memblk->size += sizeof(struct memblk_head) + next_memblk->size;
        verbose( "merge %p and %p, newsize:%x\n", curr_memblk, next_memblk, curr_memblk->size );
        p->next = p->next->next;
        p->next->prev = p;

        {   // change element in memblk_size list
            struct list_head *p1, *p2, *newp;
            p1 = &(curr_memblk->memblk_size_list_node);
            p2 = &(next_memblk->memblk_size_list_node);
            newp = &(curr_memblk->memblk_size_list_node);
            list_del(p1);
            list_del(p2);
            insert_sort_by_size(newp, &memblks_size);
        }
    }
}

void free(void *ptr)
{
    struct memblk_head *blk;

    blk = ((struct memblk_head *)ptr) - 1 ;
    blk->available = 1;

    verbose("free %p, blk at:%p\n", ptr, blk);

    try_merge( &(blk->memblk_addr_list_node) );
}

static struct memblk_head* find_first_proper_memblk(u32int size) {
    struct list_head *p;
    struct memblk_head *new_blk;

    list_for_each(p, &memblks_size) {
        new_blk = list_entry(p, struct memblk_head, memblk_size_list_node);
        if ( new_blk->available && new_blk->size >= sizeof(struct memblk_head) + size ) {
            return new_blk;
        }
    }
    return 0;
}

void *malloc(u32int size)
{
    struct memblk_head *orig_blk;
    struct memblk_head *new_blk;

    if ( size <= 0 )
        return 0;

    orig_blk = find_first_proper_memblk(size);

    if (orig_blk) {
        new_blk = split_from(orig_blk, size);
        return(void *)(new_blk+1);
    } else {
        // no proper blk
        return 0;
    }
}
