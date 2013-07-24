/*
	debug.h
	
	Определяет некоторые отладочные макросы

*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define BREAK() asm("xchg %bx, %bx")

#endif //__DEBUG_H__
