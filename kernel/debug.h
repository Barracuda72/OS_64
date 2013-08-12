#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * Некоторые отладочные макросы
 */

#define BREAK() asm("xchg %bx, %bx")

#endif //__DEBUG_H__
