/*
 * memcmp.c
 *
 * Часть стандарта C2011
 *
 * Функция сравнивает содержимое областей памяти s1 и s2
 *
 */

#include <stdint.h>
#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
  uint64_t i;
  const unsigned char *a1 = (const unsigned char *)s1;
  const unsigned char *a2 = (const unsigned char *)s2;

  for (i = 0; i < n; i++)
    if (a1[i]-a2[i])
      return a1[i] - a2[i];

  return 0;
}
