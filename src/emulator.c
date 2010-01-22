#include "emulator.h"

#include "port.h"

void bochs_shutdown()
{
    outb(0x8900, 'S');
    outb(0x8900, 'h');
    outb(0x8900, 'u');
    outb(0x8900, 't');
    outb(0x8900, 'd');
    outb(0x8900, 'o');
    outb(0x8900, 'w');
    outb(0x8900, 'n');
}

void bochs_enter_debugger()
{
    BochsConsolePrintChar('d');
    asm volatile("xchg %bx, %bx");
}
