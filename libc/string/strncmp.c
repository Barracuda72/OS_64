/*
 * strncmp.c
 *
 * Часть стандарта C2011
 *
 * Функция сравнивает содержимое строк s1 и s2
 *
 */

#include <stdint.h>
#include <string.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
  while ((*s1 != 0) && (*s2 != 0) && (*s1 == *s2) && (n > 0))
  {
    n--;
    s1++;
    s2++;
  }

  return (unsigned char)*s1 - (unsigned char)*s2;
}
