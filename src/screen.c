#include "screen.h"

#define MAX_SCREEN_NUMBER 4

struct screen_struct {
    u32 row;
    u32 col;
    u32 color;
};

static volatile char *video_mem=(volatile char*)0xB8000;
static struct screen_struct screens[MAX_SCREEN_NUMBER] = {0}; 

void screen_clear(u8 index)
{
    // assert(index<MAX_SCREEN_NUMBER)   
}

void locate(u32 row, u32 col)
{

}

void put_char(u32 color, u8 c)
{

}

void put_string(u32 color, u8 *string)
{

}
