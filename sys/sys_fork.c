/*
 * sys_fork.c
 *
 * Системный вызов fork()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_fork(void) 
{
  errno = EAGAIN;
  return -1;
}
