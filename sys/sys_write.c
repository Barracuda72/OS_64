/*
 * sys_write.c
 *
 * Системный вызов write()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_write(int file, const char *ptr, int len) 
{
  return 0;
}
