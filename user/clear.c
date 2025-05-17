#include "printf.h"

// Clear the screen
void clear()
{
  puts("\x1B[2J"); // Clear the screen
  puts("\x1B[H");  // Move cursor to home position
}