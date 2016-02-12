/*
 * strncat.c
 *
 * Часть стандарта C2011
 *
 * Функция дописывает не более n символов строки s2 в конец строки s1
 *
 */

#include <stdint.h>
#include <string.h>

char *strncat(char * restrict dest, const char * restrict src, size_t n)
{
  char *d = dest;
  uint64_t i;
  
  while (*dest++);
  for (i = 0; (i < n) && (src[i] != 0); i++)
    *dest++ = *src++;
    
  dest[i] = 0;

  return d;
}
