/*
 * sys_exit.c
 *
 * Системный вызов exit()
 *
 */

#include <syscall.h>
#include <task.h>
#include <apic.h>

#include <errno.h>

void sys_exit(int status) 
{
  curr[apic_get_id()]->status = status;
  // TODO: реально завершать процесс!
  for(;;);
}
