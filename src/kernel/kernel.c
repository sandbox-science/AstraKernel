#include <stdbool.h>
#include <stddef.h>

#include "datetime.h"
#include "printf.h"
#include "clear.h"
#include "interrupt.h"
#include "memory.h"
#include "log.h"

#ifdef USE_KTESTS
#include "tests.h"
#endif

extern char __heap_start__;
extern char __heap_end__;
extern char __heap_size__;

static const char *banner[] = {
    "========================================\r\n",
    "  AstraKernel  v0.1.0\r\n",
    "  Built " __DATE__ " at " __TIME__ "\r\n",
    "========================================\r\n",
    "\r\n",
    "  CPU: Cortex-A8 @ 200MHz (simulated)\r\n",
    "  RAM: 128MB SDRAM at 0x00000000\r\n",
    "\r\n",
    "Welcome to your own little Astra world!\r\n",
    "Type away, explore, have fun.\r\n",
    "\r\n"
};

// Initializes and prints the welcome banner.
static void init_message(void)
{
    for (size_t i = 0; i < sizeof(banner) / sizeof(banner[0]); ++i)
    {
        puts(banner[i]);
    }

    printf("AstraKernel is running...\r\n");
    printf("Press Ctrl-A and then X to exit QEMU.\r\n");
    printf("\r\n");
}

void irq_sanity_check(void)
{
    irq_disable();
    const unsigned before = systicks;

    irq_enable();
    irq_disable();

    const unsigned after = systicks;

    if (before == after)
    {
        KLOG(KLOG_INFO, "A1 Sanity PASS: no spurious IRQs");
    }
    else
    {
        KLOG(KLOG_ERROR, "A1 Sanity FAIL: unexpected IRQs");
    }

#ifdef KLOG_USE_TICKS
    irq_enable();
#endif
}

/* The following macros are for testing purposes. */
#define     TIMER_TICK_TEST     ktests_timer_test()
#define     SANITY_CHECK        irq_sanity_check()
#define     CALL_SVC_0          __asm__ volatile ("svc #0")
#define     KMALLOC_TEST        kmalloc_test()

// Entry point for the kernel
void kernel_main(void)
{
    clear();
    KLOG(KLOG_INFO, "kernel_main start");
    kmalloc_init(&__heap_start__, &__heap_end__);
    KLOG(KLOG_INFO, "kmalloc init");

#ifdef KLOG_USE_TICKS
    interrupts_init_timer0(100, 1000000);
    irq_enable();
    KLOG(KLOG_INFO, "timer0 started for log ticks");
#endif

    /* TESTS */
#ifdef USE_KTESTS 
    SANITY_CHECK;
    CALL_SVC_0;
    KMALLOC_TEST;
    TIMER_TICK_TEST;
#endif

    /* Back to normal operations */
    init_message();

    char input_buffer[100];

    dateval date_struct;
    timeval time_struct;

    bool is_running = true;
    while (is_running)
    {
        input_buffer[0] = '\0'; // Clear the input buffer
        printf("AstraKernel > ");
        getlines(input_buffer, sizeof(input_buffer));

        printf("\r\n");

        if (input_buffer[0] == '\0') continue;

        switch (input_buffer[0])
        {
            case 'h': // Check for help command
                printf("\nHelp:\n 'q' to exit\n 'h' for help\n 'c' to clear screen\n 't' to print current time\n 'd' to print current date\r\n");
                break;

            case 'b':
#ifdef USE_KTESTS_FEATURE
                #include <limits.h>
                printf("UART base: %p\n", (void *)0x101F1000);
                printf("CPU Mode: %s\n", "Supervisor");
                printf("Test signed: %d, unsigned: %u, hex: 0x%X\n", -42, 42U, 42U);
                printf("Zero: %d\n", 0);
                printf("Neg: %d\n", -1);
                printf("Max int: %d\n", INT_MAX);
                printf("Min int: %d\n", INT_MIN);   // verify no overflow crash
                printf("Unsigned max: %u\n", UINT_MAX);
                printf("Hex lower: %x\n", 0xdeadbeef);
                printf("Hex upper: %X\n", 0xdeadbeef);
                printf("Percent: %%\n");
                uint8_t number_address[1] = {0};
                uint8_t *ptr = &number_address[0];
                printf("Pointer address: %p\n", ptr);
                printf("Long decimal: %ld\n", 2147483647L);
                printf("Long unsigned: %lu\n", 4294967295UL);
                printf("unsigned: %u\n", 4294967295U);
                printf("Long hex: %lX\n", 0xFFFFFFFFL);
#else
                printf("Unknown command. Type 'h' for help.\r\n");
#endif
                break;

            case 'e':
#ifdef USE_KTESTS_FEATURE
                #include <string.h>
                int result = strcmp("abc", "abc"); // Expect 0
                printf("Expect 0 -> %d\r\n", result);

                result = strcmp("abc", "abd"); // Expect -1
                printf("Expect -1 -> %d\r\n", result);

                result = strcmp("abc", "ABC"); // Expect 1
                printf("Expect 1 -> %d\r\n", result);

                result = strcmp("ABC", "abc"); // Expect -1
                printf("Expect -1 -> %d\r\n", result);

                result = strcmp("\x01\x02\x03", "\x01\x02\x03"); // Expect 0
                printf("Expect 0 -> %d\r\n", result);

                result = strcmp("\x01\x02\x03", "\x01\x02\x04"); // Expect -1
                printf("Expect -1 -> %d\r\n", result);

                result = strcmp("\x01\x02\x04", "\x01\x02\x03"); // Expect 1
                printf("Expect 1 -> %d\r\n", result);
#else
                printf("Unknown command. Type 'h' for help.\r\n");
#endif
                break;

            case 'q': // Check for exit command
                KLOG(KLOG_INFO, "Exiting...");
                is_running = false;
                break;

            case 'c': // Check for clear screen command
                clear();
                break;

            case 't': // Check for time command
                gettime(&time_struct);
                printf("Current time(GMT): %d:%d:%d\r\n", time_struct.hrs, time_struct.mins, time_struct.secs);
                break;

            case 'd': // Check for date command
                getdate(&date_struct);
                printf("Current date(MM-DD-YYYY): %d-%d-%d\r\n", date_struct.month, date_struct.day, date_struct.year);
                break;

            default:
                printf("Unknown command. Type 'h' for help.\r\n");
                break;
        }
    }
}
