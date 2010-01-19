#include "common.h"

#define MAX_SCREEN_NUMBER 4

#define ROW_START 0
#define ROW_END 25
#define COL_START 0
#define COL_END 80
#define COL_LEN (COL_END-COL_START)

#define WHITE 0x1F

#define TAB_WIDTH 4

struct screen_struct {
	u8int enable;
    u16int row;
    u16int col;
    u8int color;
	char *mem;
};

static int current_screen_index=0;
static char *video_mem=(char*)0xB8000;
static struct screen_struct screens[MAX_SCREEN_NUMBER] = {0}; 

void screen_init_early()
{
	int index;
	struct screen_struct *p;
	
	
	screens[0].mem = video_mem;
	
	for(index=0; index<MAX_SCREEN_NUMBER; index++) {
		p = (struct screen_struct *)(&screens[index]);
		if (p->mem) {
			int row, col;
			for(row=0; row < ROW_END; row++) {
				for(col=COL_START; col < COL_END; col++) {
					// char
					*(p->mem + row*COL_LEN*2 + col*2) = 0x0;
					// color
					*(p->mem + row*COL_LEN*2 + col*2 + 1) = 
						COLOR_BRIGHT_WHITE | COLOR_BLACK << 4;
				}
			}
			p->enable=1;
			p->row=0;
			p->col=0;
			p->color=WHITE;
		}
	}
}

void screen_reset(u8int index)
{
	struct screen_struct *p;
	
    // assert(index<MAX_SCREEN_NUMBER)
    
    p = (struct screen_struct *)(&screens[index]);
	if (p->mem) {
			int row, col;
			for(row=0; row < ROW_END; row++) {
				for(col=COL_START; col < COL_END; col++) {
					// char
					*(p->mem + row*COL_LEN*2 + col*2) = 0x0;
					// color
					*(p->mem + row*COL_LEN*2 + col*2 + 1) = 
						COLOR_BRIGHT_WHITE | COLOR_BLACK << 4;
				}
			}
			p->enable=1;
			p->row=0;
			p->col=0;
			p->color= COLOR_BRIGHT_WHITE | COLOR_BLACK << 4;
	}
}

void locate_on_screen(int index, u16int row, u16int col)
{
	screens[index].row = row + ROW_START;
	screens[index].col = col;
	
	move_cursor(row, col);
}

void locate_on(u16int row, u16int col)
{
	locate_on_screen(current_screen_index, row, col);
}

void set_screen_color(int index, u8int color)
{
		screens[index].color = color;
}

void set_color(u8int color)
{
	set_screen_color(current_screen_index, color);
}

#define fix_screen_row_col(row, col, len) \
do \
{ (col) += (len); \
	if( (col) >= COL_END) { \
		(row)++; \
		if( (len) == COL_LEN ) \
			(col)=0; \
		else \
			(col) %= COL_LEN; \
	} \
} while(0)

void scroll_screen(u32int index, u16int lines)
{
	memcpy(screens[index].mem, 
		screens[index].mem + lines * COL_LEN * 2,
		(ROW_END - lines) * COL_LEN *2);
	memset(screens[index].mem + (ROW_END-1) * COL_LEN * 2, 0, 160);
}

void putchar_at_screen(int index, u8int color, u8int c)
{
	struct screen_struct *p;
	u8int need_to_show=0;
	
	p = &screens[index];
	
	switch(c) {
			case '\n':
				fix_screen_row_col(p->row, p->col, COL_LEN);
				break;
			case '\t':
				fix_screen_row_col(p->row, p->col, TAB_WIDTH);
				break;
			case '\b':
				fix_screen_row_col(p->row, p->col, -1);
				break;
			default:
				need_to_show=1;
				break;
	}
	
	if (p->row >= ROW_END) {
		scroll_screen(index, 1);
		p->row = ROW_END -1;
	}
		
	if (need_to_show) {
		*(p->mem + p->row * COL_LEN * 2 + p->col*2 ) = c;
		*(p->mem + p->row * COL_LEN * 2 + p->col*2 +1 ) = color;
		fix_screen_row_col(p->row, p->col, 1);
	}
	
	move_cursor(p->row, p->col);
}

void putchar(u8int c)
{
	putchar_at_screen(current_screen_index, screens[current_screen_index].color, c);
}


void putstring_at_screen(int index, u8int color, char *string)
{
	char *c;
	struct screen_struct *p;
	
	p = &screens[index];

	c = string;
	while(*c) {
		putchar_at_screen(index, color, *c);
		c++;
	}
}

void putstring(u8int *string)
{
	putstring_at_screen(current_screen_index, screens[current_screen_index].color, string);
}

void move_cursor(u16int row, u16int col)
{
	u16int cursorLocation;

	cursorLocation = row * COL_LEN + col;
	outb(0x3D4, 0xE);
	outb(0x3D5, cursorLocation >> 8);
	outb(0x3D4, 0xF);
	outb(0x3D5, cursorLocation);
}
