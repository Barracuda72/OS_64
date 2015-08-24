/*
 * sys_getpid.c
 *
 * Системный вызов getpid()
 *
 */

#include <syscall.h>
#include <task.h>
#include <apic.h>

#include <errno.h>

int sys_getpid(void) 
{
  return curr[apic_get_id()]->pid;
}
