/*
 * sys_close.c
 *
 * Системный вызов close()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_close(int file) 
{
  errno = EBADF;
  return -1;
}
