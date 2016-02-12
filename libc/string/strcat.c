/*
 * strcat.c
 *
 * Часть стандарта C2011
 *
 * Функция дописывает строку s2 в конец строки s1
 *
 */

#include <stdint.h>
#include <string.h>

char *strcat(char * restrict dest, const char * restrict src)
{
  char *d = dest;
  while (*dest++);
  while (*dest++ = *src++);

  return d;
}
