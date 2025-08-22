#include <stdint.h>

#include "irq.h"

volatile unsigned int irq_count = 0;

void irq_handler(void)
{
    irq_count++;
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
