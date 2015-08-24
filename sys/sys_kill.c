/*
 * sys_kill.c
 *
 * Системный вызов kill()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_kill(int pid, int sig) 
{
  errno = EINVAL;
  return -1;
}
