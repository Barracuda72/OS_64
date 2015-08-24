/*
 * sys_fstat.c
 *
 * Системный вызов fstat()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_fstat(int file, /*struct stat*/void *buf) 
{
  errno = ENOMEM;
  return -1;
}
