#ifndef SCREEN_H
#define SCREEN_H

#include "type_alias.h"

#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x1
#define COLOR_RED 0x4
#define COLOR_BRIGHT_BLUE 0x9
#define COLOR_BRIGHT_WHITE 0xF

void screen_init_early();
void screen_reset(u8int index);

void locate_on_screen(int index, u16int row, u16int col);
void locate_on(u16int row, u16int col);

void set_screen_color(int index, u8int color);
void set_color(u8int color);

void putchar_at_screen(int index, u8int color, u8int c);
void putchar(u8int c);

void putstring_at_screen(int index, u8int color, char *string);
void putstring(u8int *string);

void move_cursor(u16int row, u16int col);


#endif
