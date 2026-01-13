/**
 * @file test_kernel.c
 * @brief Kernel debug helpers for USE_KTESTS builds.
 */
#include "tests.h"
#include "interrupt.h"
#include "memory.h"
#include "printf.h"
#include "log.h"

#include <stdint.h>

extern char __heap_start__;
extern char __heap_end__;
extern char __heap_size__;

// This function is for testing purposes.
// It test that the timer interrupt is firing as expected.
void ktests_timer_test(void)
{
    KLOG(KLOG_INFO, "Memory allocation firing test!");

    void *p  = kmalloc(10);
    void *p2 = kmalloc(48);
    printf("kmalloc(10) addr: %p\n", p);
    printf("kmalloc(48) addr: %p\n", p2);
    struct header *h = (struct header *)p - 1;
    struct header *h2 = (struct header *)p2 - 1;
    printf("size of kmalloc(10) %d\nsize of kmalloc(48) %d\n", h->size, h2->size);
    kfree(p);
    kfree(p2);

    char *buf = kmalloc(5);
    if (buf)
    {
        printf("buf addr: %p\n", buf);
        buf[0] = 'a';
        buf[1] = 'b';
        buf[2] = 'c';
        printf("content -> %c%c%c\n addr -> %p\n%p\n%p\n", buf[0], buf[1], buf[2], &buf[0], &buf[1], &buf[2]);
        printf("buf[0] add: %p\n", &buf[0]);
    }

    unsigned heap_size_kb = (unsigned)(uintptr_t)&__heap_size__ / 1024;
    printf("heap: start=0x%x end=0x%x size=%u KiB\r\n",
           (unsigned)(uintptr_t)&__heap_start__,
           (unsigned)(uintptr_t)&__heap_end__,
           heap_size_kb);

    KLOG(KLOG_INFO, "Time0 IRQ firing test!");

    // Configure Timer0 for 100 Hz assuming 1 MHz timer clock in QEMU
    interrupts_init_timer0(100, 1000000);

    // Unmask CPU IRQs (I-bit = 0)
    irq_enable();

    // Test loop: print a dot every ~100 ticks
    uint64_t last = 0;
    for (;;)
    {
        if (systicks - last >= 100)
        {
            last = systicks;
            puts(".");
        }
    }
}
