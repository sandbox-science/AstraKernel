#include <stdint.h>

// Memory-mapped I/O registers for UART0 on QEMU versatilepb
#define UART0_DR (*(volatile uint32_t *)0x101f1000) // Data Register
#define UART0_FR (*(volatile uint32_t *)0x101f1018) // Flag Register
#define UART_FR_TXFF (1U << 5)                      // Transmit FIFO Full

// Simple delay loop (optional, for pacing)
void delay(volatile uint32_t count)
{
    while (count--)
        ;
}

// Send a single character over UART, waiting if the FIFO is full
void putc(char c)
{
    // Wait until UART transmit FIFO is not full
    while (UART0_FR & UART_FR_TXFF)
        ;
    UART0_DR = (uint32_t)c;

    delay(10000);
}

// Send a null-terminated string over UART
void puts(const char *s)
{
    while (*s)
    {
        putc(*s++);
    }
}

// Entry point for the kernel
void kernel_main(void)
{
    // Test individual characters
    putc('S');
    putc('A');
    putc('N');
    putc('D');
    putc('B');
    putc('O');
    putc('X');
    putc(' ');
    putc('S');
    putc('C');
    putc('I');
    putc('E');
    putc('N');
    putc('C');
    putc('E');
    putc('\r');
    putc('\n');

    // Test string output
    // TO-DO: Investigate why this doesn't work
    // puts("Hello, World!\r\n");

    while (1)
    {
        // Infinite loop to keep the kernel running
    }
}
