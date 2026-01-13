/**
 * @file input.c
 * @brief UART-backed line input with simple editing.
 */
#include "printf.h"
#include "uart.h"

#include <stdbool.h>
#include <stdint.h>

static inline void putc(char c)
{
    // Wait until UART transmit FIFO is not full
    while (UART0_FR & UART_FR_TXFF) {}
    UART0_DR = (uint32_t)c;
}

/**
 * @brief Blocking UART character input.
 *
 * @return The next character read from UART.
 */
static inline char getc(void)
{
    // Wait until UART receive FIFO is not empty
    while (UART0_FR & UART_FR_RXFE) {}
    return (char)(UART0_DR & 0xFF);
}

void getlines(char *restrict buffer, size_t length)
{
    long index = 0;
    long cursor_position = 0;

    char character;

    uint8_t escape = 0;
    uint8_t arrow_keys = 0;

    while (index < (long)length - 1)
    {
        character = getc();

        if (character == '\033') // Handle Escape sequences
        {
            escape = 1;
            continue;
        }

        if (escape)
        {
            if (escape == 1)
            {
                arrow_keys = (character == 91);
            }
            else
            {
                if (arrow_keys)
                {
                    switch (character)
                    {
                    case 'A': // Up
                    {
                        break;
                    }
                    case 'B': // Down
                    {
                        break;
                    }
                    case 'C': // Right
                    {
                        if (cursor_position < index)
                        {
                            puts("\033[C");
                            cursor_position++;
                        }
                        break;
                    }
                    case 'D': // Left
                    {
                        if (cursor_position - 1 >= 0)
                        {
                            puts("\033[D");
                            cursor_position--;
                        }
                    }
                    default:
                    {
                        break;
                    }
                    }

                    arrow_keys = 0;
                }
            }
            escape++;

            if (escape == 3) // Escape sequence is 3 characters long
            {
                escape = 0;
            }
            continue;
        }

        if (character == '\r') // Check for carriage return
        {
            break;
        }
        if (character == '\b' || character == 0x7F) // Check for backspace
        {
            if (cursor_position > 0 && index > 0) // Delete char if present
            {
                long initial_pos = cursor_position;

                for (long cur = cursor_position - 1; cur < index; cur++) // Shift characters to the left
                {
                    buffer[cur] = buffer[cur + 1];
                }

                bool cond = (index != initial_pos);

                index--;
                buffer[index] = '\0';

                cursor_position--;

                if (cond)
                {
                    printf("\033[%ldC", (index - cursor_position));
                }

                putc('\b'); // Move cursor back
                putc(' ');  // Clear the character
                putc('\b'); // Move cursor back again

                if (cond)
                {
                    printf("\033[%ldD", index - cursor_position); // Analogous to the above putc sequence, but for multiple characters
                    printf("%s", buffer + cursor_position);
                    printf("\033[%ldD", index - cursor_position);
                }
            }
        }
        else
        {
            putc(character); // Echo the character back

            long initial_pos = cursor_position;

            for (long cur = index; cur >= cursor_position; cur--) // Shift characters to the right
            {
                buffer[cur + 1] = buffer[cur];
            }

            buffer[cursor_position] = character; // Store the character in the buffer

            if (index != initial_pos)
            {
                puts(buffer + cursor_position + 1);
                printf("\033[%ldD", index - initial_pos);
            }

            cursor_position++;
            index++;
        }
    }
    buffer[index] = '\0'; // Null-terminate the string
}
