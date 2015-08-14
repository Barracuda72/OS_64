#ifndef __KLIBC_H__
#define __KLIBC_H__

#include <stdint.h>

void kprintf(const char *fmt, ...);

void *zeromem(void *mem, uint32_t n);

// Для printf
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

// Интересные макросы
#define SIGN(x) (x > 0 ? '+' : (x < 0 ? '-' : ' '))
#define ABS(x) (x > 0 ? x : (-x))

#endif //__KLIBC_H__

