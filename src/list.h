#ifndef LIST_H
#define LIST_H

struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name);

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
            pos = pos->next)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)


void INIT_LIST_HEAD(struct list_head *list);

void __list_add(struct list_head *new,
                  struct list_head *prev,
                  struct list_head *next);

void list_add(struct list_head *new, struct list_head *head);

#endif
