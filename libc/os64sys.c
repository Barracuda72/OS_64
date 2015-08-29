/*
 * os64sys.c
 *
 * Системные вызовы, интерфейс уровня пользователя
 */

#include <syscall.h>
#include <os64sys.h>

DEFN_SYSCALL2(test_out, _N_testcall, const char *, unsigned long)

DEFN_SYSCALL3(write, _N_write, int, const char *, size_t)

int write(int a, const char *b, size_t c)
{
  return syscall_write(a, b, c);
}
