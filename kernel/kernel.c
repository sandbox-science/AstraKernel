#include <stdint.h>

// Memory-mapped I/O registers for UART0 on QEMU versatilepb
#define UART0_DR (*(volatile uint32_t *)0x101f1000) // Data Register
#define UART0_FR (*(volatile uint32_t *)0x101f1018) // Flag Register
#define UART_FR_TXFF (1U << 5)                      // Transmit FIFO Full

// Send a single character over UART, waiting if the FIFO is full
void putc(char c)
{
    // Wait until UART transmit FIFO is not full
    while (UART0_FR & UART_FR_TXFF)
        ;
    UART0_DR = (uint32_t)c;
}

// Send a null-terminated string over UART
void puts(const char *s)
{
    while (*s)
    {
        putc(*s++);
    }
}

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
    puts("\x1B[2J"); // Clear the screen
    puts("\x1B[H");  // Move cursor to home position

    init_message();
    puts("Kernel is running...\r\n");
    puts("Press Ctrl-A and then X to exit QEMU.\r\n");
    puts("\r\n");

    while (1)
    {
        /* spin here */
    }
}
