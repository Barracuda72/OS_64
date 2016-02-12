/*
 * strlen.c
 *
 * Часть стандарта C2011
 *
 * Функция возвращает длину строки
 *
 */

#include <stdint.h>
#include <string.h>

size_t strlen(const char *s)
{
  size_t i = 0;
  
  while (*s++) i++;
  
  return i;
}
