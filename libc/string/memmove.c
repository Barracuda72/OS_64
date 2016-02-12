/*
 * memmove.c
 *
 * Часть стандарта C2011
 *
 * Функция копирует содержимое области памяти s2 в область памяти s1 
 * так, будто это происходит посредством третьего промежуточного буфера
 *
 */

#include <stdint.h>
#include <string.h>

void *memmove(void * dest, const void * src, size_t n)
{
  char *d = (char *)dest;
  char *s = (char *)src;
  
  uint64_t i;
  
  // Если начало d лежит внутри s, то нужно производить копирование 
  // с конца
  if ((s+n > d) && (s < d))
  {
	  for (i = n; i > 0; i--)
	    d[i-1] = s[i-1];
  } else {
    for(i = 0; i < n; i++)
      d[i] = s[i];
  }
  return dest;
}
