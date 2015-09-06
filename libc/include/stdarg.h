/*
 * stdarg.h
 *
 * Часть стандарта C2011
 *
 * Работа со списком неизвестных аргументов
 *
 */

#ifndef _STDARG_H
#define _STDARG_H 1

#if 1
/*
 * Нижележащее специфично для GCC
 */

typedef __builtin_va_list va_list;

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

#else

/*
 * Реализация для общего случая
 */

typedef void * va_list;

#define va_start(v,l) \
  _va_start(&v, (void *)l, sizeof(l))

#define va_arg(v,l) \
  (l)_va_arg(&v, sizeof(l))

#define va_end(v) \
  _va_end(&v)

#define va_copy(d,s) \
  _va_copy(&d,&s)

/*
 * Инициализирует список параметров начальным значением
 */
void _va_start(va_list *ap, void *p, int size);

/*
 * Возвращает следующий аргумент из списка аргументов переменной длины
 */
void *_va_arg(va_list *ap, int size);

/*
 * Уничтожает список аргументов переменной длины
 */
void _va_end(va_list *ap);

/*
 * Копирует список аргументов переменной длины
 */
void _va_copy(va_list *ap, va_list *bp);

#endif

#endif // _STDARG_H
