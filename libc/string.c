/*
 * string.c
 *
 * Строковые функции
 *
 */

#include <stdint.h>
#include <string.h> 

char *strncpy(char *dest, const char *src, size_t len)
{
  int i;
  for(i = 0; (i < len)&&(src[i] != 0); i++)
    dest[i] = src[i];
  for(i; i < len; i++)
    dest[i] = 0;
  return dest;
}

size_t strlen(const char *s)
{
  int i;
  for (i = 0; s[i] != 0; i++);
  return i;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  int i = 0;
  while ((s1[i] == s2[i]) && (s1[i] != 0) && (i < (n-1))) // N-1, да
    i++;

  return s1[i] - s2[i];
}

char *strncat(char *dest, const char *src, size_t n)
{
  uint32_t dest_len = strlen(dest);
  uint32_t i;

  for (i = 0 ; i < n && src[i] != '\0' ; i++)
    dest[dest_len + i] = src[i];

  dest[dest_len + i] = '\0';

  return dest;
}


void *memset(void *s, int c, size_t n)
{ 
  uint8_t *s1 = s;
  uint32_t i;
  for (i = 0; i < n; i++)
    s1[i] = (char)c;
  return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
  char *d = (char *)dest;
  char *s = (char *)src;
  uint32_t i;
  for(i = 0; i < n; i++)
    d[i] = s[i];

  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  size_t i;
  const unsigned char *a1 = (const unsigned char *)s1;
  const unsigned char *a2 = (const unsigned char *)s2;

  for (i = 0; i < n; i++)
    if (a1[i]-a2[i])
      return a1[i] - a2[i];

  return 0;
}

