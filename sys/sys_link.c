/*
 * sys_link.c
 *
 * Системный вызов link()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_link(const char *oldpath, const char *newpath) 
{
  errno = EMLINK;
  return -1;
}
