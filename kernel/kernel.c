#include <stdbool.h>
#include <stddef.h>

#include "datetime.h"
#include "printf.h"
#include "clear.h"
#include "string.h"

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
    printf("AstraKernel is running...\r\n");
    printf("Press Ctrl-A and then X to exit QEMU.\r\n");
    printf("\r\n");

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

        switch (input_buffer[0])
        {
            case 'h': // Check for help command
                printf("\nHelp:\n 'q' to exit\n 'h' for help\n 'c' to clear screen\n 't' to print current time\n 'd' to print current date\r\n");
                break;
            case 'e':
                int result = strcmp("abc", "abc"); // Expect 0
                printf("Expect 0 -> %d\n", result);

                result = strcmp("abc", "abd"); // Expect -1
                printf("Expect -1 -> %d\n", result);

                result = strcmp("abc", "ABC"); // Expect 1
                printf("Expect 1 -> %d\n", result);

                result = strcmp("ABC", "abc"); // Expect -1
                printf("Expect -1 -> %d\n", result);

                result = strcmp("\x01\x02\x03", "\x01\x02\x03"); // Expect 0
                printf("Expect 0 -> %d\n", result);

                result = strcmp("\x01\x02\x03", "\x01\x02\x04"); // Expect -1
                printf("Expect -1 -> %d\n", result);

                result = strcmp("\x01\x02\x04", "\x01\x02\x03"); // Expect 1
                printf("Expect 1 -> %d\n", result);
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
                printf("Current time(GMT): %d:%d:%d\n", time_struct.hrs, time_struct.mins, time_struct.secs);
                break;
            case 'd': // Check for date command
                getdate(&date_struct);
                printf("Current date(MM-DD-YYYY): %d-%d-%d\n", date_struct.month, date_struct.day, date_struct.year);
                break;
            default:
                printf("Unknown command. Type 'h' for help.\r\n");
                break;
        }
    }
}
