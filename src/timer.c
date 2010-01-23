#include "timer.h"
#include "irq.h"
#include "register.h"

u32int tick=0;

static void timer_callback(struct registers regs)
{
    tick++;
    putchar_at_screen_row_col(0, 0, 78, COLOR_BLUE | COLOR_BLACK<<4, tick%10+48);
    //printf_bochs("%x\n", tick);
}

void init_timer(u32int freq)
{
    u32int divisor = 1193180 / freq;

    register_irq_service(IRQ0, &timer_callback);

    outb(0x43, 0x36);

    u8int l = (u8int)(divisor & 0xFF);
    u8int h = (u8int)( (divisor>>8) & 0xFF );

    outb(0x40, l);
    outb(0x40, h);
}
