#include <stdbool.h>

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
        puts(banner[i]);
    }
}

// Entry point for the kernel
void kernel_main(void)
{
    clear();

    init_message();
    puts("AstraKernel is running...\r\n");
    puts("Press Ctrl-A and then X to exit QEMU.\r\n");
    puts("\r\n");

    char input_buffer[100];

    bool is_running = true;
    while (is_running)
    {
        input_buffer[0] = '\0'; // Clear the input buffer
        puts("AstraKernel > ");
        getlines(input_buffer, sizeof(input_buffer));

        puts("\r\n");

        switch (input_buffer[0])
        {
        case 'h': // Check for help command
            puts("\nHelp:\n 'q' to exit\n 'h' for help\n 'c' to clear screen\n 't' to print current time\n 'd' to print current date\r\n");
            break;
        case 'e':
            puts("%ld %ld %ld\n", 0, -9223372036854775808, 9223372036854775807);
            puts("%d %d\n", 2147483647, -2147483648);
            puts("%x %lx %lX %X\n", 2147483647, 2147483649, 2147483648, 1234);
            puts("%lX %x %lx\n", 0x123456789abcdef0, 1234, 9223372036854775809);
            puts("Name: %c\n", 'b');
            puts("Hello %s\n", "World");
            puts("100%%\n");
            break;
        case 'q': // Check for exit command
            puts("Exiting...\r\n");
            is_running = false;
            break;
        case 'c': // Check for clear screen command
            clear();
            break;
        case 't':                               // Check for time command
            puts("Current time: 12:00 PM\r\n"); // TO-DO: Implement real time check
            break;
        case 'd':                                 // Check for date command
            puts("Current date: 2023-10-01\r\n"); // TO-DO: Implement real date check
            break;
        default:
            puts("Unknown command. Type 'h' for help.\r\n");
            break;
        }
    }
}
