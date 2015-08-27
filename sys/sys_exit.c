/*
 * sys_exit.c
 *
 * Системный вызов exit()
 *
 */

#include <syscall.h>
#include <task.h>

#include <errno.h>

void sys_exit(int status) 
{
  curr_cpu_task->status = status;
  // TODO: реально завершать процесс!
  for(;;);
}
