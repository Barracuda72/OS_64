/*
 * uchar.h
 *
 * Часть стандарта C2011
 *
 * Вспомогательные процедуры Unicode
 *
 */

#ifndef _UCHAR_H
#define _UCHAR_H 1

#include <stdint.h> // Для uint_least*

#include <decl/size_t.h>
#include <decl/mbstate_t.h>

typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;

/*
 * Преобразует некоторый символ строки, возможно, задаваемый параметром 
 * ps, в 16-битный символ Unicode
 */
size_t mbrtoc16(char16_t * restrict pc16,
                const char * restrict s, size_t n,
                mbstate_t * restrict ps);

#endif // _UCHAR_H
