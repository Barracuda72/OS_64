/*
 * strncpy.c
 *
 * Часть стандарта C2011
 *
 * Функция копирует содержимое строки s2 в строку s1, дополняя нулями,
 * если это необходимо
 *
 */

#include <stdint.h>
#include <string.h>

char *strncpy(char * restrict dest, const char * restrict src, size_t n)
{
  uint64_t i;
  
  for(i = 0; (i < n) && (src[i] != 0); i++)
    dest[i] = src[i];
    
  for(i; i < n; i++)
    dest[i] = 0;
    
  return dest;
}
