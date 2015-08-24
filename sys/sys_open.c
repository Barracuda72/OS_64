/*
 * sys_open.c
 *
 * Системный вызов open()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_open(const char *name, int flags, int mode) 
{
  errno = ENOMEM;
  return -1;
}
