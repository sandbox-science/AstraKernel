#include "printf.h"

// Clears the terminal screen and moves the cursor to the home position.
void clear(void)
{
  puts("\x1B[2J");
  puts("\x1B[H");
}