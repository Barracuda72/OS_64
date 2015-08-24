/*
 * sys_execve.c
 *
 * Системный вызов execve()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_execve(const char *name, char *const argv[], char *const envp[]) 
{
  errno = ENOMEM;
  return -1;
}
