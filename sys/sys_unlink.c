/*
 * sys_unlink.c
 *
 * Системный вызов unlink()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_unlink(const char *name) 
{
  errno = ENOENT;
  return -1;
}
