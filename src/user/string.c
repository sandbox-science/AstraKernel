#include "string.h"

int strcmp(const char *str_1, const char *str_2)
{
  unsigned char ch1, ch2;
  do
  {
    ch1 = *str_1++;
    ch2 = *str_2++;

    if (ch1 != ch2)
    {
      // NOTE: If needed in the future, can return the difference between
      // the strings rather than just -1 or 1
      return ch1 < ch2 ? -1 : 1;
    }

  } while (ch1);

  return 0;
}

size_t strlen(const char *str)
{
  const char *s = str;
  while (*s)
  {
    ++s;
  }
  return s - str;
}
