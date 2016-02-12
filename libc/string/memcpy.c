/*
 * memcpy.c
 *
 * Часть стандарта C2011
 *
 * Функция копирует содержимое области памяти s2 в область памяти s1
 *
 */

#include <stdint.h>
#include <string.h>

void *memcpy(void * restrict dest, const void * restrict src, size_t n)
{
  char *d = (char *)dest;
  char *s = (char *)src;
  uint64_t i;
  for(i = 0; i < n; i++)
    d[i] = s[i];

  return dest;
}
