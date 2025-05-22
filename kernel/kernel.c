#include <stdbool.h>

#include "printf.h"
#include "clear.h"
#include "datetime.h"

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

    timeval time_struct;
    dateval date_struct;

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
            puts("%lx %ld %ld\n", 18446744073709551615, -9223372036854775809, 9223372036854775809);
            puts("%d %d\n", 2147483647, -2147483647);
            puts("%x %x %X %X\n", 2147483647, 1234, 2147483647, 1234);
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
        case 't': // Check for time command
            gettime(&time_struct);
            puts("Current time(GMT): %d:%d:%d\n", time_struct.hrs, time_struct.mins, time_struct.secs);
            break;
        case 'd': // Check for date command
            getdate(&date_struct);
            puts("Current date(MM-DD-YYYY): %d-%d-%d\n", date_struct.month, date_struct.day, date_struct.year);
            break;
        default:
            puts("Unknown command. Type 'h' for help.\r\n");
            break;
        }
    }
}
