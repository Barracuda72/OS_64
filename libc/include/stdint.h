/*
 * stdint.h
 *
 * Часть стандарта C2011
 *
 * Целочисленные типы
 *
 */

#ifndef _STDINT_H
#define _STDINT_H 1

/*
 * Типы точной длины
 */

/*
 * Знаковые
 */
typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;
typedef signed long  int64_t;

/*
 * Беззнаковые
 */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

/*
 * Типы минмиальной длины
 */

/*
 * Знаковые
 */
typedef signed char  int_least8_t;
typedef signed short int_least16_t;
typedef signed int   int_least32_t;
typedef signed long  int_least64_t;

/*
 * Беззнаковые
 */
typedef unsigned char  uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int   uint_least32_t;
typedef unsigned long  uint_least64_t;

/*
 * Типы максимального быстродействия
 */

/*
 * Знаковые
 */
typedef signed char int_fast8_t;
typedef signed long int_fast16_t;
typedef signed long int_fast32_t;
typedef signed long int_fast64_t;

/*
 * Беззнаковые
 */
typedef unsigned char uint_fast8_t;
typedef unsigned long uint_fast16_t;
typedef unsigned long uint_fast32_t;
typedef unsigned long uint_fast64_t;

/*
 * Типы для указателей
 */
typedef unsigned long uintptr_t;
typedef   signed long intptr_t;

/*
 * Типы максимальной длины
 */
typedef unsigned long uintmax_t;
typedef   signed long intmax_t;

/*
 * Пределы для типов фиксированной длины
 */
#define INT8_MIN  (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483648)
#define INT64_MIN (-9223372036854775808L)

#define INT8_MAX  (127)
#define INT16_MAX (32767)
#define INT32_MAX (2147483647)
#define INT64_MAX (9223372036854775807L)

#define UINT8_MAX  (255)
#define UINT16_MAX (65535)
#define UINT32_MAX (4294967295U)
#define UINT64_MAX (18446744073709551615UL)

/*
 * Пределы для типов минимальной длины
 */
#define INT_LEAST8_MIN  (-128)
#define INT_LEAST16_MIN (-32768)
#define INT_LEAST32_MIN (-2147483648)
#define INT_LEAST64_MIN (-9223372036854775808L)

#define INT_LEAST8_MAX  (127)
#define INT_LEAST16_MAX (32767)
#define INT_LEAST32_MAX (2147483647)
#define INT_LEAST64_MAX (9223372036854775807L)

#define UINT_LEAST8_MAX  (255)
#define UINT_LEAST16_MAX (65535)
#define UINT_LEAST32_MAX (4294967295U)
#define UINT_LEAST64_MAX (18446744073709551615UL)

/*
 * Пределы для быстрых типов
 */
#define INT_FAST8_MIN  (-128)
#define INT_FAST16_MIN (-9223372036854775808L)
#define INT_FAST32_MIN (-9223372036854775808L)
#define INT_FAST64_MIN (-9223372036854775808L)

#define INT_FAST8_MAX  (127)
#define INT_FAST16_MAX (9223372036854775807L)
#define INT_FAST32_MAX (9223372036854775807L)
#define INT_FAST64_MAX (9223372036854775807L)

#define UINT_FAST8_MAX  (255)
#define UINT_FAST16_MAX (18446744073709551615UL)
#define UINT_FAST32_MAX (18446744073709551615UL)
#define UINT_FAST64_MAX (18446744073709551615UL)

/*
 * Пределы для типов указателей
 */
#define INTPTR_MIN    (-9223372036854775808L)
#define INTPTR_MAX    (9223372036854775807L)
#define UINTPTR_MAX   (18446744073709551615UL)

/*
 * Пределы для типов максимальной длины
 */
#define INTMAX_MIN   (-9223372036854775808L)
#define INTMAX_MAX   (9223372036854775807L)
#define UINTMAX_MAX  (18446744073709551615UL)

/*
 * Пределы прочих типов
 */
#define PTRDIFF_MIN (-9223372036854775808L)
#define PTRDIFF_MAX (9223372036854775807L)

/*
 * Тип не предоставляется libc, но объявить необходимо
 */
#define SIG_ATOMIC_MIN (0)
#define SIG_ATOMIC_MAX (255)

#define SIZE_MAX    (18446744073709551615UL)

/*
 * Считаем wchar_t беззнаковым
 */
#define WCHAR_MIN (0)
#define WCHAR_MAX (65535)
#define WCHAR_MIN 0

/*
 * Пределы типа wint_t
 */
#define WINT_MIN   (0U)
#define WINT_MAX   (4294967295U)

/*
 * Макросы для инициализации целочисленных констант
 */

#define  INT8_C(c)  c
#define INT16_C(c) c
#define INT32_C(c) c
#define INT64_C(c)  c ## L

#define UINT8_C(c) c
#define UINT16_C(c)  c
#define UINT32_C(c)  c ## U
#define UINT64_C(c) c ## UL

#define INTMAX_C(c) c ## L
#define UINTMAX_C(c)  c ## UL

#endif// _STDINT_H
