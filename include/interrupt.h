#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// PL190 VIC (interrupt controller)
// ref: PrimeCell Vectored Interrupt Controller (PL190) TRM (Page3-7)
#define VIC_BASE        0x10140000u
#define VIC_INTSELECT   (*(volatile uint32_t *)(VIC_BASE + 0x00C)) // 0=IRQ,1=FIQ
#define VIC_INTENABLE   (*(volatile uint32_t *)(VIC_BASE + 0x010)) // set bit=enable
#define VIC_INT_ENCLR   (*(volatile utin32_t *)(VIC_BASE + 0x014))
#define VIC_SOFT_INT    (*(volatile uint32_t *)(VIC_BASE + 0x018))
#define VIC_SOFT_INTCLR (*(volatile uint32_t *)(VIC_BASE + 0x01C))

// SP804 Timer0 in the 0/1 block
// ref: ARM Dual-Time Module (SP804) TRM (Page 3-2)
#define T01_BASE    0x101E2000u
#define T0_LOAD     (*(volatile uint32_t *)(T01_BASE + 0x00))
#define T0_VALUE    (*(volatile uint32_t *)(T01_BASE + 0x04))
#define T0_CONTROL  (*(volatile uint32_t *)(T01_BASE + 0x08))
#define T0_INTCLR   (*(volatile uint32_t *)(T01_BASE + 0x0C))
#define T0_MIS      (*(volatile uint32_t *)(T01_BASE + 0x14))

// Bits for CONTROL (SP804)
// ref: ARM Dual-Time Module (SP804) TRM (Page 3-5)
#define TCTRL_ENABLE    (1u << 7)   // EN=bit7
#define TCTRL_PERIODIC  (1u << 6)   // PERIODIC=bit6
#define TCTRL_INTEN     (1u << 5)   // INTEN=bit5
#define TCTRL_32BIT     (1u << 1)   // 32BIT=bit1

// VIC line number for Timer0/1 on Versatile
#define IRQ_TIMER01 4

    static inline void timer0_start_periodic(uint32_t load)
    {
        T0_CONTROL  = 0;        // disable while reconfig
        T0_LOAD     = load;     // ex: 10000 for ~100 Hz if TIMCLK≈1 MHz
        T0_INTCLR   = 1;        // clear any pending interrupt
        T0_CONTROL  = TCTRL_32BIT | TCTRL_PERIODIC | TCTRL_INTEN | TCTRL_ENABLE;
    }

    static inline void vic_enable_timer01_irq(void)
    {
        VIC_INTSELECT   &= ~(1u << IRQ_TIMER01);  // route to IRQ
        VIC_INTENABLE   |=  (1u << IRQ_TIMER01);  // enable line 4
    }
#ifdef __cplusplus
}
#endif

#endif // INTERRUPT_H
