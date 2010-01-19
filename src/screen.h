#ifndef SCREEN_H
#define SCREEN_H

#include "type_alias.h"

void screen_clear(u8 index);
void locate(u32 row, u32 col);
void put_char(u32 color, u8 c);
void put_string(u32 color, u8 *string);

#endif
