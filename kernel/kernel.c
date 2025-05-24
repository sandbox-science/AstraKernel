#include <stdbool.h>
#include <stddef.h>

#include "printf.h"
#include "clear.h"

static const char *banner[] = {
    "========================================\r\n",
    "  AstraKernel  v0.1.0\r\n",
    "  Built " __DATE__ " at " __TIME__ "\r\n",
    "========================================\r\n",
    "\r\n",
    "  CPU: ARM926EJ-S @ 200MHz (simulated)\r\n",
    "  RAM: 128MB SDRAM at 0x00000000\r\n",
    "\r\n",
    "Welcome to your own little Astra world!\r\n",
    "Type away, explore, have fun.\r\n",
    "\r\n"};

// Initializes and prints the welcome banner.
static void init_message(void)
{
    for (size_t i = 0; i < sizeof(banner) / sizeof(banner[0]); ++i)
    {
        printf(banner[i]);
    }
}

// Entry point for the kernel
void kernel_main(void)
{
    clear();

    init_message();
    printf("AstraKernel is running...\r\n");
    printf("Press Ctrl-A and then X to exit QEMU.\r\n");
    printf("\r\n");

    char input_buffer[100];

    bool is_running = true;
    while (is_running)
    {
        input_buffer[0] = '\0'; // Clear the input buffer
        printf("AstraKernel > ");
        getlines(input_buffer, sizeof(input_buffer));

        printf("\r\n");

        switch (input_buffer[0])
        {
        case 'h': // Check for help command
            printf("\nHelp:\n 'q' to exit\n 'h' for help\n 'c' to clear screen\n 't' to print current time\n 'd' to print current date\r\n");
            break;
        case 'e':
            printf("Char and String: %c %s\n", 'b', "HELLO WORLD");
            printf("Longs: %d %ld %ld\n", 0, -9223372036854775808, 9223372036854775807);
            printf("%lu %lu\n", 9223372036854775807, 18446744073709551615);
            printf("%d %d\n", 2147483647, -2147483648);
            printf("%x %x %X %X\n", 2147483647, 1234, 2147483647, 1234);
            printf("%lX %x %lx\n", 0x123456789abcdef0, 1234, 9223372036854775809);
            printf("Name: %c\n", 'b');
            printf("Hello %s\n", NULL);
            printf("100%%s\n");
            printf("Percent: %");
            printf("\n");
            printf("%d\n", 10, 10, 1, 11);
            break;
        case 'q': // Check for exit command
            printf("Exiting...\r\n");
            is_running = false;
            break;
        case 'c': // Check for clear screen command
            clear();
            break;
        case 't':                                 // Check for time command
            printf("Current time: 12:00 PM\r\n"); // TO-DO: Implement real time check
            break;
        case 'd':                                   // Check for date command
            printf("Current date: 2023-10-01\r\n"); // TO-DO: Implement real date check
            break;
        default:
            printf("Unknown command. Type 'h' for help.\r\n");
            break;
        }
    }
}
