/*
 * memset.c
 *
 * Часть стандарта C2011
 *
 * Функция заполняет область памяти указанным символом
 *
 */

#include <stdint.h>
#include <string.h>

void *memset(void *s, int c, size_t n)
{ 
  uint8_t *src = s;
  uint64_t i;
  
  for (i = 0; i < n; i++)
    src[i] = (uint8_t)c;
  
  return s;
}
