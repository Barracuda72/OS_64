/*
 * sys_wait.c
 *
 * Системный вызов wait()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_wait(int *status) 
{
  errno = ECHILD;
  return -1;
}
