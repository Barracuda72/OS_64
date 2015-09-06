/*
 * stddef.h
 *
 * Часть стандарта C2011
 *
 * Общие определения
 *
 */

#ifndef _STDDEF_H
#define _STDDEF_H 1

typedef signed   long  ptrdiff_t;
#include <decl/size_t.h> // Для size_t
typedef unsigned long  max_align_t;
typedef unsigned short wchar_t;

#include <decl/NULL.h>

#define offsetof(type, memder) \
  ( (size_t) & ((type*)0) -> member )

#endif // _STDDEF_H
