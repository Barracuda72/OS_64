/*
 * sys_times.c
 *
 * Системный вызов times()
 *
 */

#include <syscall.h>
#include <errno.h>

int sys_times(/*struct tms*/void *buf) 
{
  return -1;
}
