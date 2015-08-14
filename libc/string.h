/*
 * string.h
 *
 * Строковые функции
 *
 */
 
#ifndef __STRING_H__
#define __STRING_H__

// Для size_t
#include <stdint.h>

size_t strlen(const char *s);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncat(char *dest, const char *src, size_t n);
char *strncpy(char *dest, const char *src, size_t len);

void *memset(void *mem, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // __STRING_H__
