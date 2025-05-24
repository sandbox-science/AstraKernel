#include "printf.h"
#include <stdint.h>
#include <stddef.h>

_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

// Memory-mapped I/O registers for UART0 on QEMU versatilepb
#define UART0_DR (*(volatile uint32_t *)0x101f1000) // Data Register
#define UART0_FR (*(volatile uint32_t *)0x101f1018) // Flag Register

static const uint32_t UART_FR_TXFF = (1U << 5); // Transmit FIFO Full
static const uint32_t UART_FR_RXFE = (1U << 4); // Receive FIFO Empty

// Send a single character over UART, waiting if the FIFO is full
static inline void putc(char c)
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
static inline char getc(void)
{
    // Wait until UART receive FIFO is not empty
    while (UART0_FR & UART_FR_RXFE)
        ;
    return (char)(UART0_DR & 0xFF);
}

// Function to getline from user input
void getlines(char *restrict buffer, size_t length)
{
    size_t index = 0;
    char character;
    while (index < length - 1)
    {
        character = getc();
        if (character == '\r') // Check for carriage return
        {
            break;
        }
        if ((character == '\b' || character == 0x7F) && index > 0) // Check for backspace
        {
            index--;
            putc('\b'); // Move cursor back
            putc(' ');  // Clear the character
            putc('\b'); // Move cursor back again
        }
        else
        {
            buffer[index++] = character;    // Store the character in the buffer
            putc(character);                // Echo the character back
        }
    }
    buffer[index] = '\0'; // Null-terminate the string
}