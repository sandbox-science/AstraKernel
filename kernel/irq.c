#include <stdint.h>

#include "irq.h"
#include "interrupt.h"

volatile unsigned int tick = 0;

void irq_handler(void)
{
    T0_INTCLR = 1; // this ack/clear the device (SP804)
    tick++;
}

inline void irq_disable(void)
{
    __asm__ volatile("cpsid i" ::: "memory"); // mask IRQ
}

inline void irq_enable(void)
{
    __asm__ volatile("cpsie i" ::: "memory"); // unmask IRQ
    __asm__ volatile("isb" ::: "memory");     // take effect immediately
}
