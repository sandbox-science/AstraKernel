#include "printf.h"

/**
 * @brief Clears the terminal screen and moves the cursor to the home position.
 */
void clear(void)
{
  printf("\x1B[2J");
  printf("\x1B[H");
}