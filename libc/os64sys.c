/*
 * os64sys.c
 *
 * Системные вызовы, интерфейс уровня пользователя
 */

#include <os64sys.h>

DEFN_SYSCALL2(test_out, 0, const char *, unsigned long)
