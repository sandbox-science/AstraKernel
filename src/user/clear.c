#include "printf.h"

void clear(void)
{
  printf("\x1B[2J");
  printf("\x1B[H");
}