#ifndef IRQ_H
#define IRQ_H

#include "register.h"

#define IRQ0 0x20
#define IRQ1 0x21
#define IRQ2 0x22
#define IRQ3 0x23
#define IRQ4 0x24
#define IRQ5 0x25
#define IRQ6 0x26
#define IRQ7 0x27
#define IRQ8 0x28
#define IRQ9 0x29
#define IRQ10 0x2a
#define IRQ11 0x2b
#define IRQ12 0x2c
#define IRQ13 0x2d
#define IRQ14 0x2e
#define IRQ15 0x2f

typedef void(*irq_service_t)(struct registers);

void register_irq_service(u8int irq_nr, irq_service_t irq_service_fn);
void initialize_irq_services();

#endif
