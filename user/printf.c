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

// Function to get user input from UART
char getc(void)
{
    // Wait until UART receive FIFO is not empty
    while (UART0_FR & (1U << 4))
        ;
    return (char)(UART0_DR & 0xFF);
}

// Function to getline from user input
void getlines(char *buffer, int length)
{
    int index = 0;
    char character;
    while (index < length - 1)
    {
        character = getc();
        if (character == '\r') // Check for carriage return
        {
            break;
        }
        if (character == '\b') // Check for backspace
        {
            if (index > 0)
            {
                index--;
                putc('\b'); // Move cursor back
                putc(' ');  // Clear the character
                putc('\b'); // Move cursor back again
            }
        }
        else
        {
            buffer[index++] = character;    // Store the character in the buffer
            putc(character);                // Echo the character back
        }
    }
    buffer[index] = '\0'; // Null-terminate the string
}