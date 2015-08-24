/*
 * sys_stat.c
 *
 * Системный вызов stat()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_stat(const char *file, /*struct stat*/void *buf) 
{
  errno = ENOMEM;
  return -1;
}
