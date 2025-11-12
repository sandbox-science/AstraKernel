#include "interrupt.h"
#include "lib/math.h"

#include <stdint.h>

volatile uint64_t systicks = 0;

void interrupts_init_timer0(uint32_t tick_hz, uint32_t timer_clk_hz)
{
    if (tick_hz == 0 || timer_clk_hz == 0)
    {
        tick_hz      = 100;      // fallback to 100 Hz
        timer_clk_hz = 1000000;  // fallback to 1 MHz
    }

    // Periodic mode reloads LOAD each cycle.
    uint32_t load = _udiv32(timer_clk_hz, tick_hz);
    if (load == 0)
    {
        load = 1;
    }

    // Program timer0 periodic
    timer0_start_periodic(load);

    // Route timer01 interrupt to IRQ and enable it in VIC
    vic_enable_timer01_irq();
}

void irq_handler(void)
{
    // Check Timer0 MIS (masked interrupt status)
    if (T0_MIS)
    {
        T0_INTCLR = 1;  // Clear the timer interrupt
        systicks++;
    }

    // End of interrupt for PL190 VIC
    VIC_VECTADDR = 0; // signal end of IRQ service
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
