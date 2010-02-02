#ifndef EMULATOR_H
#define EMULATOR_H

#define BOCHS_DEBUGGER_ENTER do { asm volatile("xchg %bx, %bx"); } while(0)

#define BochsConsolePrintChar(c) do { outb(0xe9, c); } while(0)
#define BochsBreak() do { outw(0x8A00,0x8A00); outw(0x8A00,0x08AE0); } while(0)

void bochs_shutdown();
void bochs_enter_debugger();

#endif
