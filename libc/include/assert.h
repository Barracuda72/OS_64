/*
 * assert.h
 *
 * Часть стандарта C2011
 *
 * Макросы отладки
 *
 */

#ifndef _ASSERT_H
#define _ASSERT_H 1

#ifdef NDEBUG

#define assert(ignore) ((void)0)

#else

#include <stdio.h>

#define assert(expression) \
  do { \
    if ((expression) == 0) { \
      dprintf(stderr, "Assertion failed: '%s', in %s at %s:%d\n", \
        #expression, __func__, __FILE__, __LINE__); \
        abort(); \
    } \
  } while (0)

#endif

#define static_assert(x) _Static_assert

#endif // _ASSERT_H
