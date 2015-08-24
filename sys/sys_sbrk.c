/*
 * sys_sbrk.c
 *
 * Системный вызов sbrk()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_sbrk(int incr) 
{
  errno = ENOMEM;
  return -1;
}
