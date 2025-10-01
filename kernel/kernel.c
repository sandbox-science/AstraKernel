#include <stdbool.h>
#include <stddef.h>

#include "datetime.h"
#include "printf.h"
#include "clear.h"
#include "string.h"
#include "interrupt.h"
#include "memory.h"

extern volatile uint64_t systicks; // this is declared in interrupt.c

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
        puts("\r\nA1 Sanity PASS: no spurious IRQs\r\n");
    }
    else
    {
        puts("\r\nA1 Sanity FAIL: unexpected IRQs\r\n");
    }
}

// This function is for testing purposes.
// It test that the timer interrupt is firing as expected.
void not_main(void)
{
    puts("Memory allocation firing test!\r\n");

    void *p = kmalloc(48);
    printf("kmalloc(48) -> 0x%x\r\n", (unsigned)(uintptr_t)p);

    unsigned heap_size_kb = (unsigned)(uintptr_t)&__heap_size__ / 1024;
    printf("heap: start=0x%x end=0x%x size=%u KiB\r\n",
           (unsigned)(uintptr_t)&__heap_start__,
           (unsigned)(uintptr_t)&__heap_end__,
           heap_size_kb);

    puts("Time0 IRQ firing test!\r\n");

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

/* The following macros are for testing purposes. */
#define     TIMER_TICK_TEST     not_main()
#define     SANITY_CHECK        irq_sanity_check()
#define     CALL_SVC_0          __asm__ volatile ("svc #0")

// Entry point for the kernel
void kernel_main(void)
{
    clear();
    kmalloc_init(&__heap_start__, &__heap_end__);
    kmalloc_remaining();

    /* TESTS */
    #ifdef USE_KTESTS 
        SANITY_CHECK;
        CALL_SVC_0;
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

            case 'e':
                /* @todo Remove this section when testings are over. */
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
                break;

            case 'q': // Check for exit command
                printf("Exiting...\r\n");
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
