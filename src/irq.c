#include "common.h"
#include "irq.h"
#include "register.h"

irq_service_t irq_services[256];

void irq_default(struct registers reg)
{
    printf("irq service is not been implemented.\n");
}

void register_irq_service(u8int irq_nr, irq_service_t irq_service_fn)
{
    irq_services[irq_nr] = irq_service_fn;
}

void initialize_irq_services()
{
    int i;

    for(i = 32; i < 47; i++)
        register_irq_service(i, &irq_default);

    register_irq_service(IRQ7, 0);
}
