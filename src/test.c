#include "test.h"

#include "mem.h"
void test_mem()
{
    int i;
    void *ptr;

    printf_bochs("test memory\n");
    printf_bochs("-------------------------------------------------\n");
    printf_bochs("Before test_mem\n");
    print_memblk_list(1);
    for(i=0;i<25;i++) {
        ptr = malloc(2*(i%5));
        if(ptr)
            free(ptr);
    }
    printf_bochs("After test_mem\n");
    print_memblk_list(1);
    printf_bochs("\n");
    printf_bochs("-------------------------------------------------\n");
}
