#ifndef PORT_H
#define PORT_H

#include "type_alias.h"

void outb(u16int port, u8int value);

u8int inb(u16int port);

u16int inw(u16int port);

#endif /* PORT_H */
