/*
 * sys_getpid.c
 *
 * Системный вызов getpid()
 *
 */

#include <syscall.h>
#include <task.h>

#include <errno.h>

int sys_getpid(void) 
{
  return curr_cpu_task->pid;
}
