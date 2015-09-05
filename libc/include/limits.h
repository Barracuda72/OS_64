/*
 * limits.h
 *
 * Часть стандарта C2011
 *
 * Ограничения типов
 *
 */

#ifndef _LIMITS_H
#define _LIMITS_H 1

/*
 * Количество бит в самом маленьком объекте
 */
#define CHAR_BIT 8

/*
 * Минимальное значение объекта типа signed char
 */
#define SCHAR_MIN (-128)

/*
 * Максимальное значение объекта типа signed char
 */
#define SCHAR_MAX (+127)

/*
 * Максимальное значение объекта типа unsigned char
 */
#define UCHAR_MAX 255

/*
 * Минимальное значение объекта типа char
 */
#define CHAR_MIN SCHAR_MIN 

/*
 * Максимальное значение объекта типа char
 */
#define CHAR_MAX SCHAR_MAX 

/*
 * Максимальное количество байт в символе многобайтной кодировки
 */
#define MB_LEN_MAX 1

/*
 * Минимальное значение объекта типа short int
 */
#define SHRT_MIN (-32768)

/*
 * Максимальное значение объекта типа short int
 */
#define SHRT_MAX (+32767)

/*
 * Максимальное значение объекта типа unsigned short int
 */
#define USHRT_MAX 65535

/*
 * Минимальное значение объекта типа int
 */
#define INT_MIN (-2147483648)

/*
 * Максимальное значение объекта типа int
 */
#define INT_MAX (+2147483647)

/*
 * Максимальное значение объекта типа unsigned int
 */
#define UINT_MAX 4294967295

/*
 * Минимальное значение объекта типа long int
 */
#define LONG_MIN (-9223372036854775808)

/*
 * Максимальное значение объекта типа long int
 */
#define LONG_MAX (+9223372036854775807)

/*
 * Максимальное значение объекта типа unsigned long int
 */
#define ULONG_MAX 18446744073709551615

/*
 * Минимальное значение объекта типа long long int
 */
#define LLONG_MIN LONG_MIN

/*
 * Максимальное значение объекта типа long long int
 */
#define LLONG_MAX LONG_MAX

/*
 * Максимальное значение объекта типа unsigned long long int
 */
#define ULLONG_MAX ULONG_MAX


#endif // _LIMITS_H
