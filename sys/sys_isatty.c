/*
 * sys_isatty.c
 *
 * Системный вызов isatty()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_isatty(int file) 
{
  return 1;
}
