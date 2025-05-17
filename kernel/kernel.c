#include "../user/printf.h"
#include "../user/clear.h"

void init_message(void)
{
    puts("========================================\r\n");
    puts("  AstraKernel  v0.1.0\r\n");
    puts("  Built " __DATE__ " at " __TIME__ "\r\n");
    puts("========================================\r\n");
    puts("\r\n");
    puts("  CPU: ARM926EJ-S @ 200MHz (simulated)\r\n");
    puts("  RAM: 128MB SDRAM at 0x00000000\r\n");
    puts("\r\n");
    puts("Welcome to your own little Astra world!\r\n");
    puts("Type away, explore, have fun.\r\n");
    puts("\r\n");
}

// Entry point for the kernel
void kernel_main(void)
{
    clear();

    init_message();
    puts("Kernel is running...\r\n");
    puts("Press Ctrl-A and then X to exit QEMU.\r\n");
    puts("\r\n");

    char input_buffer[100];

    while (1)
    {
        input_buffer[0] = '\0'; // Clear the input buffer
        puts("AstraKernel > ");
        getlines(input_buffer, sizeof(input_buffer));
        
        puts("\r\n");

        if (input_buffer[0] == 'q')         // Check for exit command
        {
            puts("Exiting...\r\n");
            break;
        }
        else if (input_buffer[0] == 'h')    // Check for help command
        {
            puts("\nHelp: Press 'q' to exit, 'h' for help.\r\n");
        }
        else if (input_buffer[0] == 'c')    // Check for clear command
        {
            clear();
        }
        else if (input_buffer[0] == 't')    // Check for time command
        {
            puts("Current time: 12:00 PM\r\n"); // TO-DO: Implement real time check
        }
        else if (input_buffer[0] == 'd')    // Check for date command
        {
            puts("Current date: 2023-10-01\r\n"); // TO-DO: Implement real date check
        }
        else
        {
            puts("Unknown command. Type 'h' for help.\r\n");
        }

    }
}
