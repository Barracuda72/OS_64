/*
 * string.h
 *
 * Часть стандарта C2011
 *
 * Работа со строками и областями памяти
 * 
 */

#ifndef _STRING_H
#define _STRING_H 1

#ifndef __IGNORE_UNIMPLEMENTED_STRING

#error Not implemented

#endif // __IGNORE_UNIMPLEMENTED_STRING

#include <decl/size_t.h>
#include <decl/NULL.h>

/*
 * Копирует содержимое области памяти s2 в область памяти s1
 */
void *memcpy(void * restrict dest, const void * restrict src, size_t n);

/*
 * Копирует содержимое области памяти s2 в область памяти s1 
 * так, будто это происходит посредством третьего промежуточного буфера
 */
void *memmove(void * dest, const void * src, size_t n);

/*
 * Копирует содержимое строки s2 в строку s1
 */
char *strcpy(char * restrict dest, const char * restrict src);

/*
 * Копирует содержимое строки s2 в строку s1, дополняя нулями,
 * если это необходимо
 */
char *strncpy(char * restrict dest, const char * restrict src, size_t n);

/*
 * Дописывает строку s2 в конец строки s1
 */
char *strcat(char * restrict dest, const char * restrict src);

/*
 * Дописывает не более n символов строки s2 в конец строки s1
 */
char *strncat(char * restrict dest, const char * restrict src, size_t n);

/*
 * Сравнивает содержимое областей памяти s1 и s2
 */
int memcmp(const void *s1, const void *s2, size_t n);

/*
 * Сравнивает содержимое строк s1 и s2
 */
int strcmp(const char *s1, const char *s2);

/*
 * Сравнивает содержимое строк s1 и s2 с учетом настроек локали
 */
int strcoll(const char *s1, const char *s2);

/*
 * Сравнивает содержимое строк s1 и s2
 */
int strncmp(const char *s1, const char *s2, size_t n);

/*
 * Преобразует строку s2 в s1 так, чтобы результат применения strcmp 
 * на двух модифицированных строках совпадал с результатом применения
 * strcoll на немодифицированных
 */
size_t strxfrm(char * restrict s1, const char * restrict s2, size_t n);

/*
 * ...
 */
 
/*
 * Заполняет область памяти указанным символом
 */
void *memset(void *s, int c, size_t n);

/*
 * Возвращает строку-описание ошибки по ее коду
 */
char *strerror(int errnum);

/*
 * Возвращает длину строки
 */
size_t strlen(const char *s);

#endif // _STRING_H
